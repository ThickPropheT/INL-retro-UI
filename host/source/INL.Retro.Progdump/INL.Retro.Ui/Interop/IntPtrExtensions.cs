using System;
using System.Runtime.InteropServices;

namespace INL.Retro.Progdump.Interop
{
    public static class IntPtrExtensions
    {
        public static T ToStruct<T>(this IntPtr ptr) where T : struct
        {
            var t = new T();
            t = Marshal.PtrToStructure<T>(ptr);
            return t;
        }

        //private void StructToPtr<T>(Action<IntPtr> callback) where T : struct
        //{
        //    var @struct = new T();
        //    var ptr = Marshal.AllocHGlobal(Marshal.SizeOf<T>());
        //    Marshal.StructureToPtr(@struct, ptr, false);

        //    try
        //    {
        //        callback(ptr);
        //    }
        //    finally
        //    {
        //        Marshal.FreeHGlobal(ptr);
        //    }
        //}
    }
}
