using System;
using System.IO;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace WindowsFormsApplication1
{
    public partial class WowTraces : Form
    {
        string ChosenFile = "";
        string path = Environment.CurrentDirectory;
        bool first = true;
        char[] delimiter = {','};
        float Xref = 0;
        float Yref = 0;
        float X = 0;
        float fZoom = 1;
        float Y = 0;
        static int size = 917;
        float Rot = 0;
        Bitmap TraceBitmap = new Bitmap(size, size);
        Bitmap SaveBitmap;

        public WowTraces()
        {
            InitializeComponent();
        }

        private void mnuLoad_Click(object sender, EventArgs e)
        {
            openFD.FileName = "";
            openFD.Title = "Select log file.";
            openFD.InitialDirectory = path;
            openFD.Filter = "LOG FILE|*.txt";


            if (openFD.ShowDialog() != DialogResult.Cancel)
            {
                ChosenFile = openFD.FileName;
            }
            else
            {
                ChosenFile = "";
            }

        }

        private void button1_Click(object sender, EventArgs e)
        {

            SaveBitmap = new Bitmap(int.Parse(nWidth.Value.ToString()), int.Parse(nHeigth.Value.ToString()));
            Graphics G = Graphics.FromImage(SaveBitmap);
            G.Clear(iTraces.BackColor);
           
            G.Dispose();

            if (ChosenFile == "")//make sure a file is chosen
            {
                MessageBox.Show("Please select a file first.");
            }
            else
            {
                //sets the zoom level according to option chosen
                if(rbZoom1.Checked)
                {
                    fZoom = 1;
                }
                else if (rbZoom2.Checked)
                {
                    fZoom = 2;
                }
                else if (rbZoom3.Checked)
                {
                    fZoom = 3;
                }
                else if (rbZoom4.Checked)
                {
                    fZoom = 4;
                }
                else if (rbZoom05.Checked)
                {
                    fZoom = 0.5F;
                }
                else if (rbZoom5.Checked)
                {
                    fZoom = 5;
                }

                try
                {

                    using (StreamReader sr = new StreamReader(ChosenFile))
                    {
                        String line;
                        while ((line = sr.ReadLine()) != null)
                        {
                            string[] info = line.Split(delimiter);
                            if (info.Length == 6)
                            {
                                if (first)
                                {
                                    first = false;
                                    Xref = float.Parse(info[2]);
                                    Yref = float.Parse(info[3]);
                                    lXref.Text = "Xref :" + Xref;
                                    lYref.Text = "Yref :" + Yref;
                                }
                                X = float.Parse(info[2]);
                                Y = float.Parse(info[3]);
                                Rot = float.Parse(info[5]);

                                TraceBitmap = SketchPlayer(TraceBitmap, Color.Blue, (Xref - X)*fZoom + iTraces.Width / 2, (Yref - Y)*fZoom + iTraces.Height / 2, Rot);
                                SaveBitmap = SketchPlayer(SaveBitmap, Color.Blue, (Xref - X) * fZoom + SaveBitmap.Width / 2, (Yref - Y) * fZoom + SaveBitmap.Height / 2, Rot);
                            }
                            else
                            {

                            }
                        }
                    }

                }
                catch (Exception ex)
                {
                    MessageBox.Show("File could not be read.");
                }
                iTraces.Image = TraceBitmap;
            }
            
        }

        private void bAllTraces_Click(object sender, EventArgs e)
        {
            string path = Environment.CurrentDirectory;
            DirectoryInfo dir = new DirectoryInfo(path);
            SaveBitmap = new Bitmap(int.Parse(nWidth.Value.ToString()), int.Parse(nHeigth.Value.ToString()));
            Graphics G = Graphics.FromImage(SaveBitmap);
            G.Clear(iTraces.BackColor);
            
            G.Dispose();
            
            //sets the zoom level according to option chosen
            if (rbZoom1.Checked)
            {
                fZoom = 1;
            }
            else if (rbZoom2.Checked)
            {
                fZoom = 2;
            }
            else if (rbZoom3.Checked)
            {
                fZoom = 3;
            }
            else if (rbZoom4.Checked)
            {
                fZoom = 4;
            }
            else if (rbZoom05.Checked)
            {
                fZoom = 0.5F;
            }
            else if (rbZoom5.Checked)
            {
                fZoom = 5;
            }

            foreach (FileInfo f in dir.GetFiles("*.txt"))
            {
                try
                {
                    String name = f.Name;

                    using (StreamReader sr = new StreamReader(name))
                    {
                        String line;
                        while ((line = sr.ReadLine()) != null)
                        {
                            string[] info = line.Split(delimiter);  //small test to ensure right format of log
                            if (info.Length == 6)
                            {
                                if(first)
                                {
                                    first = false;
                                    Xref = float.Parse(info[2]);
                                    Yref = float.Parse(info[3]);
                                    lXref.Text = "Xref :" + Xref;
                                    lYref.Text = "Yref :" + Yref;
                                }
                                X = float.Parse(info[2]);
                                Y = float.Parse(info[3]);
                                Rot = float.Parse(info[5]);

                                TraceBitmap = SketchPlayer(TraceBitmap, Color.Blue, (Xref - X)*fZoom + iTraces.Width/2, (Yref - Y)*fZoom + iTraces.Height/2, Rot);
                                SaveBitmap = SketchPlayer(SaveBitmap, Color.Blue, (Xref - X) * fZoom + SaveBitmap.Width / 2, (Yref - Y) * fZoom + SaveBitmap.Height / 2, Rot);
                            }
                            else
                            {

                            }
                        }
                    }

                }
                catch (Exception ex)
                {
                    MessageBox.Show("File could not be read.");
                }

            }
            iTraces.Image = TraceBitmap;

        }

        private void bClear_Click(object sender, EventArgs e)
        {
            first = true;
            Xref = 0;
            Yref = 0;
            Graphics G = Graphics.FromImage(TraceBitmap);
            G.Clear(iTraces.BackColor);
            //G.Clear(Color.SeaShell);
            G.Dispose();

            Graphics G1 = Graphics.FromImage(SaveBitmap);
            G1.Clear(iTraces.BackColor);
            G1.Dispose();

            iTraces.Image = TraceBitmap;
        }

        private float RadToDeg(float Rot)
        {
            return (float)(Rot * (180 / Math.PI));
        }

        private Bitmap SketchPlayer(Bitmap img, Color UnitColor, float Ypos, float Xpos, float Rotation)
        {
            Graphics G = Graphics.FromImage(img);
            G.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
            Pen TracePen = new Pen(UnitColor, 1F);

            Rotation = RadToDeg(Rotation);
            G.ResetTransform();     //Resets the world transformation matrix of this Graphics to the identity matrix.
            G.TranslateTransform(-Xpos, -Ypos, System.Drawing.Drawing2D.MatrixOrder.Append);        //move drawing coordinates to origin
            try
            {
                G.RotateTransform(-Rotation, System.Drawing.Drawing2D.MatrixOrder.Append);          //rotate counterclockwise at origin
            }
            catch (ArgumentException)
            {
            }
            G.TranslateTransform(Xpos, Ypos, System.Drawing.Drawing2D.MatrixOrder.Append);          //move back to drawing coordinates
            try
            {
                if(rbArrow.Checked)
                {
                    G.DrawLine(TracePen, Xpos - 2, Ypos + 1, Xpos, Ypos - 3);
                    G.DrawLine(TracePen, Xpos + 2, Ypos + 1, Xpos, Ypos - 3);
                    G.DrawLine(TracePen, Xpos, Ypos, Xpos, Ypos - 3);
                }
                else if(rbLine.Checked)
                {
                    G.DrawLine(TracePen, Xpos, Ypos, Xpos, Ypos - 3);
                }

            }
            catch (Exception ex)
            {

            }

            G.Dispose();
            TracePen.Dispose();

            return img;
        }

        private void btnSave_Click(object sender, EventArgs e)
        {
            SaveBitmap.Save("traces.bmp", System.Drawing.Imaging.ImageFormat.Bmp);
        }


    }
}
