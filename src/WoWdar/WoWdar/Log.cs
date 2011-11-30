using System;
using System.IO;
using System.Collections.Generic;
using System.Collections;
using System.Linq;
using System.Text;

namespace WoWdar
{
    class Log
    {

        public void WriteToFile(ArrayList datalist)
        {
            foreach(ObjArray obj in datalist)
            {
                using (StreamWriter w = File.AppendText(obj.GUID + ".txt"))
                {
                    foreach(TimeAndPos data in obj.info)
                    {
                        w.WriteLine(data.time.ToString("yyyy/MM/dd,HH:mm:ss.ffff") + "," + data.XPos + "," + data.YPos + ","+ data.ZPos + "," + data.RotPos);     //write data to log file
                    }
                    w.Flush();  //write and clear all buffered text
                    w.Close();  //close file
                }
            }
        }



        /*public ObjArray LoadFromFiles()     //is this neccessary? Could just append the file. Ask John
        {
            ObjArray load = new ObjArray();
            string path = Environment.CurrentDirectory;
            DirectoryInfo dir = new DirectoryInfo(path);

            foreach (FileInfo f in dir.GetFiles("*.txt"))
            {
                String name = f.Name;
                name = name.Replace(".txt", "");
                ulong lGuid = UInt64.Parse(name);
               
            }


            return load;
        }*/

    }
}
