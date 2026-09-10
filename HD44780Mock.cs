using System;
using Antmicro.Renode.Core;
using Antmicro.Renode.Peripherals;
using Antmicro.Renode.Logging;

namespace Antmicro.Renode.Peripherals.Miscellaneous
{
    public class HD44780Mock : IPeripheral, IGPIOReceiver
    {
        public HD44780Mock()
        {
            ddram = new char[0x80];
            Reset();
        }

        public void Reset()
        {
            for(var i = 0; i < ddram.Length; i++)
            {
                ddram[i] = ' ';
            }

            rs = false;
            enable = false;

            d4 = false;
            d5 = false;
            d6 = false;
            d7 = false;

            waitingForLowNibble = false;
            fourBitMode = false;
            highNibble = 0;

            address = 0;
        }

        /*
         * GPIO mapping:
         *
         * 0 = RS
         * 1 = E
         * 2 = D4
         * 3 = D5
         * 4 = D6
         * 5 = D7
         */
        public void OnGPIO(int number, bool value)
        {
            switch(number)
            {
                case 0:
                    rs = value;
                    break;

                case 1:
                    // HD44780 chốt dữ liệu ở cạnh xuống của E
                    if(enable && !value)
                    {
                        LatchNibble();
                    }

                    enable = value;
                    break;

                case 2:
                    d4 = value;
                    break;

                case 3:
                    d5 = value;
                    break;

                case 4:
                    d6 = value;
                    break;

                case 5:
                    d7 = value;
                    break;
            }
        }

        private void LatchNibble()
        {
            byte nibble = 0;

            if(d4) nibble |= 0x01;
            if(d5) nibble |= 0x02;
            if(d6) nibble |= 0x04;
            if(d7) nibble |= 0x08;

            /*
             * HD44780 4-bit initialization:
             * firmware sends 0x3, 0x3, 0x3, then 0x2 as single nibbles.
             * Do not pair these nibbles as normal bytes.
             */
            if(!fourBitMode && !rs)
            {
                if(nibble == 0x03)
                {
                    waitingForLowNibble = false;
                    return;
                }

                if(nibble == 0x02)
                {
                    fourBitMode = true;
                    waitingForLowNibble = false;
                    return;
                }
            }

            if(!waitingForLowNibble)
            {
                highNibble = nibble;
                waitingForLowNibble = true;
                return;
            }

            byte value = (byte)((highNibble << 4) | nibble);

            waitingForLowNibble = false;

            if(rs)
            {
                ProcessData(value);
            }
            else
            {
                ProcessCommand(value);
            }
        }

        private void ProcessCommand(byte command)
        {
            /*
             * Clear display
             */
            if(command == 0x01)
            {
                ClearDisplay();
                address = 0;
                return;
            }

            /*
             * Cursor home
             */
            if(command == 0x02)
            {
                address = 0;
                return;
            }

            /*
             * Set DDRAM address
             *
             * 0x80 -> dòng 1
             * 0xC0 -> dòng 2
             */
            if((command & 0x80) != 0)
            {
                address = (byte)(command & 0x7F);
                return;
            }

            /*
             * Các command khác như:
             *
             * 0x28 4-bit / 2-line
             * 0x0C display ON
             * 0x06 entry mode
             *
             * không cần mô phỏng chi tiết cho project này.
             */
        }

        private void ProcessData(byte data)
        {
            /*
             * DDRAM dòng 1:
             * 0x00 -> 0x0F
             *
             * DDRAM dòng 2:
             * 0x40 -> 0x4F
             */

            if(address <= 0x0F)
            {
                ddram[address] = (char)data;
            }
            else if(address >= 0x40 && address <= 0x4F)
            {
                ddram[address] = (char)data;
            }

            address++;
        }

        private void ClearDisplay()
        {
            for(var i = 0; i < ddram.Length; i++)
            {
                ddram[i] = ' ';
            }
        }

        public string GetLine1()
        {
            return new string(ddram, 0x00, 16);
        }

        public string GetLine2()
        {
            return new string(ddram, 0x40, 16);
        }

        /*
         * Gõ trong Renode:
         *
         * gpioPortB.LCD16x2 Show
         */
        public void Show()
        {
            this.Log(
                LogLevel.Info,
                "\n+----------------+\n|{0}|\n|{1}|\n+----------------+",
                GetLine1(),
                GetLine2()
            );
        }

        private readonly char[] ddram;

        private bool rs;
        private bool enable;

        private bool d4;
        private bool d5;
        private bool d6;
        private bool d7;

        private bool waitingForLowNibble;
        private bool fourBitMode;

        private byte highNibble;
        private byte address;
    }
}