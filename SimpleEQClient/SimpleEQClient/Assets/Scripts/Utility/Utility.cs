using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Runtime.InteropServices;
using System.Runtime.Serialization.Formatters.Binary;


namespace Scripts.Utility
{
    class Utility
    {
        public static byte[] ToByteArray(object source)
        {
            int datasize = Marshal.SizeOf(source);
            IntPtr buff = Marshal.AllocHGlobal(datasize);
            Marshal.StructureToPtr(source, buff, false);
            byte[] data = new byte[datasize];
            Marshal.Copy(buff, data, 0, datasize);
            Marshal.FreeHGlobal(buff);

            return data;
        }

        public static object ByteArrayToObject(byte[] data, Type type)
        {
            IntPtr buff = Marshal.AllocHGlobal(data.Length);
            Marshal.Copy(data, 0, buff, data.Length);
            object obj = Marshal.PtrToStructure(buff, type);
            Marshal.FreeHGlobal(buff);

            return obj;
        }

        public static void StringToCharArray(char[] dest, String source)
        {
            var len = dest.Length;
            var str_len = source.Length;


            int i = 0;

            for (; i < str_len; i++)
            {
                dest[i] = source[i];
            }

            for (; i<len;i++)
            {
                dest[i] = '\0';
            }
        }
    }
}
