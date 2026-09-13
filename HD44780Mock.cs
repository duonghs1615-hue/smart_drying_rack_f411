using System;
using Antmicro.Renode.Core;
using Antmicro.Renode.Peripherals;
using Antmicro.Renode.Peripherals.I2C;
using Antmicro.Renode.Logging;

namespace Antmicro.Renode.Peripherals.Miscellaneous
{
    public class HD44780I2CMock : II2CPeripheral
    {
        public HD44780I2CMock()
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

            waitingForLowNibble = false;
            fourBitMode = false;

            highNibble = 0;
            address = 0;
        }

        /*
         * PCF8574 mapping used by lcd.c:
         *
         * P0 -> RS
         * P1 -> RW
         * P2 -> EN
         * P3 -> Backlight
         * P4 -> D4
         * P5 -> D5
         * P6 -> D6
         * P7 -> D7
         */
        public void Write(byte[] data)
        {
            foreach(var value in data)
            {
                ProcessPCF8574Byte(value);
            }
        }

        /*
         * Firmware currently does not read data from the LCD.
         * Return zeroes if a read is requested.
         */
        public byte[] Read(int count = 1)
        {
            return new byte[count];
        }

        public void FinishTransmission()
        {
        }

        private void ProcessPCF8574Byte(byte value)
        {
            bool newRs = (value & 0x01) != 0;
            bool newEnable = (value & 0x04) != 0;

            /*
             * P4-P7 contain the LCD nibble.
             */
            byte nibble = (byte)((value >> 4) & 0x0F);

            rs = newRs;

            /*
             * HD44780 latches data on the falling edge of EN.
             */
            if(enable && !newEnable)
            {
                LatchNibble(nibble);
            }

            enable = newEnable;
        }

        private void LatchNibble(byte nibble)
        {
            /*
             * HD44780 initialization sequence:
             *
             * 0x3
             * 0x3
             * 0x3
             * 0x2
             *
             * These are single nibbles and must not be paired.
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
                    waitingForLowNibibbleReset();
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

        private void waitingForLowNibibbleReset()
        {
            waitingForLowNibble = false;
        }

        private void ProcessCommand(byte command)
        {
            /*
             * Clear display.
             */
            if(command == 0x01)
            {
                ClearDisplay();
                address = 0;
                return;
            }

            /*
             * Cursor home.
             */
            if(command == 0x02)
            {
                address = 0;
                return;
            }

            /*
             * Set DDRAM address.
             *
             * 0x80 -> line 1
             * 0xC0 -> line 2
             */
            if((command & 0x80) != 0)
            {
                address = (byte)(command & 0x7F);
                return;
            }

            /*
             * Other commands used by this project:
             *
             * 0x28 -> 4-bit, 2-line
             * 0x0C -> display ON
             * 0x06 -> increment cursor
             *
             * No detailed simulation is required for them.
             */
        }

        private void ProcessData(byte data)
        {
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
         * Renode command:
         *
         * lcdI2C Show
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

        private bool waitingForLowNibble;
        private bool fourBitMode;

        private byte highNibble;
        private byte address;
    }
}