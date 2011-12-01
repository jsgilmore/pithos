using System;
using System.Collections.Generic;
using System.Collections;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using Magic;

namespace WoWdar
{
    public partial class WoWdar : Form
    {
        uint ClientCon = 0;
        uint ObjectMan = 0;
        uint FirstObj = 0;
        uint TotWowObj = 0;
        uint WoWBase = 0;
        bool TrackerReady = false;
        ArrayList allObjects = new ArrayList();
        ArrayList objGuid = new ArrayList();
        bool Found = false;
         
        /// <summary>
        /// This block of variables is for drawing positions on radar.
        /// </summary>
        static int DisplayHeight = 552;
        static int DisplayWidth = 552;
        Bitmap TrackingBitmap = new Bitmap(DisplayWidth, DisplayHeight);
        float TZoom = 1;        //default zoom level
        //uint rZoom = 10;            //default value of radioboxes variable
        int absZRange = 100;        //dont show units that are 100 higher or lower than yourself

        //color definitions
        Color PlayerColor = Color.Blue;
        Color DeadPlayer = Color.DarkBlue;
        Color LiveNPC = Color.Plum;
        Color DeadNPC = Color.Gray;
        //Color PetColor = Color.LightBlue;
        //Color PetDeadColor = Color.Aquamarine;
        Color LocalPlayer = Color.ForestGreen;
        Color CurrentTarget = Color.Red;
        //Color NodeColor = Color.Gold;




        WoWObject Player = new WoWObject();
        WoWObject Me = new WoWObject();
        WoWObject Target = new WoWObject();
        WoWObject CurrentObj = new WoWObject();
        WoWObject TempObj = new WoWObject();
        BlackMagic wow = new BlackMagic();


        public WoWdar()
        {
            InitializeComponent();
        }


        private void WoWdar_Load(object sender, EventArgs e)
        {
            try
            {
                if (LoadOffsets() == true)
                    TrackerReady = true;

                if (TrackerReady == false)
                {
                    ClearBitmap(ref TrackingBitmap);
                    TrackingBitmap = WriteTextCenter(TrackingBitmap, "Please enter the game world and click Reload.", 8);
                    display.Image = TrackingBitmap;
                }
            }
            catch(Exception)
            {
                ClearBitmap(ref TrackingBitmap);
                TrackingBitmap = WriteTextCenter(TrackingBitmap, "Could not load offsets. Please Reload.", 8);
                display.Image = TrackingBitmap;
            }        
        }

        private Boolean LoadOffsets()
        {
            //kyk na GetProcessesFromWindowTitle om 'n lys te kry van processes met PID
            if (wow.OpenProcessAndThread(SProcess.GetProcessFromProcessName("Wow")))//Getting procces from name Wow(Multilanguage) 
            {

            }
            else
            {
                return false;
            }

            IntPtr baseWoW = wow.MainModule.BaseAddress;//Gets Base Address
            WoWBase = (uint)baseWoW;


            ClientCon = wow.ReadUInt((uint)baseWoW + (uint)ObjectManagerOff.clientConnection);
            ObjectMan = wow.ReadUInt(ClientCon + (uint)ObjectManagerOff.objectManager);
            FirstObj = wow.ReadUInt(ObjectMan + (uint)ObjectManagerOff.firstObject);
            Target.GUID = wow.ReadUInt64((uint)baseWoW + (uint)PlayerOff.LastTargetGUID);
            Me.GUID = wow.ReadUInt64(ObjectMan + (uint)ObjectManagerOff.localGuid);
            Me.Name = wow.ReadASCIIString((uint)baseWoW + 0x980598, 20);

            
            this.pName.Text = "Name: " + Me.Name;
            this.pGuid.Text = "GUID: " + Me.GUID;


            if (Me.GUID == 0)
                return false;
            else
                return true;

        }

        private float RadToDeg(float Rot)
        {
            return (float)(Rot * (180 / Math.PI));
        }

        private void RadarTimer_Tick(object sender, EventArgs e)
        {
            if(rZoom1.Checked)
            {
                TZoom = 1;
            }
            else if(rZoom2.Checked)
            {
                TZoom = 2;
            }
            else if (rZoom3.Checked)
            {
                TZoom = 3;
            }
            else if (rZoom4.Checked)
            {
                TZoom = 4;
            }

            //TZoom = (rZoom / 10);      //Zoom level of radar

            if(!cbShowTrace.Checked) //don't clear bitmap if traces must be shown
            {
                ClearBitmap(ref TrackingBitmap);
            }



            if (TrackerReady == false)
            {
                TrackingBitmap = WriteTextCenter(TrackingBitmap, "Please enter the game world then click Reload.", 8);
                display.Image = TrackingBitmap;
                return;
            }

            try
            {
                TotWowObj = 0;
                CurrentObj.BaseAddress = FirstObj;
                Target.GUID = wow.ReadUInt64(WoWBase + (uint)PlayerOff.LastTargetGUID);

                Me.BaseAddress = GetBaseByGuid(Me.GUID);
                Me.X = wow.ReadFloat(Me.BaseAddress + (uint)ObjectOffsets.Pos_X);
                Me.Y = wow.ReadFloat(Me.BaseAddress + (uint)ObjectOffsets.Pos_Y);
                Me.Z = wow.ReadFloat(Me.BaseAddress + (uint)ObjectOffsets.Pos_Z);
                Me.Rot = wow.ReadFloat(Me.BaseAddress + (uint)ObjectOffsets.Rot);

                Graphics g = Graphics.FromImage(TrackingBitmap);

                if (Target.GUID != 0)    //if valid target, get target details
                {
                    Target.BaseAddress = GetBaseByGuid(Target.GUID);
                    Target.X = wow.ReadFloat(Target.BaseAddress + (uint)ObjectOffsets.Pos_X);
                    Target.Y = wow.ReadFloat(Target.BaseAddress + (uint)ObjectOffsets.Pos_Y);
                    Target.Z = wow.ReadFloat(Target.BaseAddress + (uint)ObjectOffsets.Pos_Z);
                    Target.Rot = wow.ReadFloat(Target.BaseAddress + (uint)ObjectOffsets.Rot);
                    Target.ObjectFields = wow.ReadUInt(Target.BaseAddress + (uint)ObjectOffsets.ObjectFields);
                    Target.Type = wow.ReadShort(Target.BaseAddress + 0x14);
                    //Target.health = wow.ReadUInt(Target.ObjectFields + (uint)UnitFields.UNIT_FIELD_HEALTH);

                    if (Target.Type == 3)    //NPC 
                    {
                        Target.Name = NPCNameFromGuid(Target.GUID);
                        //Target.Name = "(NPC) " + Target.Name;       //debugging purposes
                    }
                    if (Target.Type == 4)    //human player
                    {
                        Target.Name = PlayerNameFromGuid(Target.GUID);
                    }
                    if ((Target.Type != 4) && (Target.Type != 3))
                    {
                        Target.Name = "Object.";
                    }

                }//end if Target Guid !=0

                //now go through all objects and add new ones to array list and update old ones (all for logging purposes).
                //also display all objects

                while ((CurrentObj.BaseAddress != 0) && (CurrentObj.BaseAddress % 2 == 0))
                {
                    TotWowObj += 1;     //count all wow objects in range

                    CurrentObj.GUID = GetGuidByBase(CurrentObj.BaseAddress);
                    CurrentObj.ObjectFields = wow.ReadUInt(CurrentObj.BaseAddress + (uint)ObjectOffsets.ObjectFields);
                    CurrentObj.Type = wow.ReadShort(CurrentObj.BaseAddress + 0x14);
                    CurrentObj.X = wow.ReadFloat(CurrentObj.BaseAddress + (uint)ObjectOffsets.Pos_X);
                    CurrentObj.Y = wow.ReadFloat(CurrentObj.BaseAddress + (uint)ObjectOffsets.Pos_Y);
                    CurrentObj.Z = wow.ReadFloat(CurrentObj.BaseAddress + (uint)ObjectOffsets.Pos_Z);
                    CurrentObj.Rot = wow.ReadFloat(CurrentObj.BaseAddress + (uint)ObjectOffsets.Rot);
                    CurrentObj.health = wow.ReadUInt(CurrentObj.ObjectFields + (uint)UnitFields.UNIT_FIELD_HEALTH);

                    if (CurrentObj.Type == 3)    //NPC 
                    {
                        CurrentObj.Name = NPCNameFromGuid(CurrentObj.GUID);
                        //CurrentObj.Name = "(NPC) " + CurrentObj.Name;       //debugging purposes
                    }
                    if (CurrentObj.Type == 4)    //human player
                    {
                        CurrentObj.Name = PlayerNameFromGuid(CurrentObj.GUID);

                    }

                    //this checks if target is within reasonable vertical range of me, and is not myself 
                    //also includes target in case target is human and needs to be logged.
                    if ((Math.Abs(Me.Z - CurrentObj.Z) <= absZRange) && (CurrentObj.GUID != Me.GUID))         
                    {                                                                                                                          
                        switch (CurrentObj.Type)
                        {
                            case 3: //npc

                                if (!cbShowTrace.Checked)   //only draw NPC if showtrace not checked
                                {

                                    if (CurrentObj.health <= 0)
                                    {
                                        TrackingBitmap = SketchPlayer(TrackingBitmap, DeadNPC, (Me.X - CurrentObj.X) * TZoom + DisplayWidth / 2, (Me.Y - CurrentObj.Y) * TZoom + DisplayHeight / 2, CurrentObj.Rot, CurrentObj.Name);
                                    }//NPC dead
                                    else
                                    {
                                        TrackingBitmap = SketchPlayer(TrackingBitmap, LiveNPC, (Me.X - CurrentObj.X) * TZoom + DisplayWidth / 2, (Me.Y - CurrentObj.Y) * TZoom + DisplayHeight / 2, CurrentObj.Rot, CurrentObj.Name);
                                    }//NPC alive

                                }                                
                                break;

                            case 4: //a player

                                if(CurrentObj.health <= 0)
                                {
                                    TrackingBitmap = SketchPlayer(TrackingBitmap, DeadPlayer, (Me.X - CurrentObj.X) * TZoom + DisplayWidth / 2, (Me.Y - CurrentObj.Y) * TZoom + DisplayHeight / 2, CurrentObj.Rot, CurrentObj.Name);
                                }//player dead
                                else
                                {       //track living human players
                                    if(cbTrack.Checked)
                                    {
                                        if (allObjects.Count <= 0)      //first entry into array
                                        {
                                            TimeAndPos temp = new TimeAndPos();
                                            temp.XPos = CurrentObj.X;
                                            temp.YPos = CurrentObj.Y;
                                            temp.ZPos = CurrentObj.Z;
                                            temp.RotPos = CurrentObj.Rot;
                                            temp.time = DateTime.Now;       //resolution of 10 milliseconds apparently
                                            ObjArray trackPlayer = new ObjArray();
                                            trackPlayer.GUID = CurrentObj.GUID;
                                            trackPlayer.info.Add(temp);
                                            allObjects.Add(trackPlayer);
                                        }
                                        else                    //search through elements in array to see if the current player is already in the array
                                        {
                                            foreach(ObjArray tracking in allObjects)
                                            {
                                                if (tracking.GUID == CurrentObj.GUID) //means current player GUID already in array
                                                {
                                                    Found = true;
                                                    TimeAndPos temp = new TimeAndPos();
                                                    temp.XPos = CurrentObj.X;
                                                    temp.YPos = CurrentObj.Y;
                                                    temp.ZPos = CurrentObj.Z;
                                                    temp.RotPos = CurrentObj.Rot;
                                                    temp.time = DateTime.Now;
                                                    tracking.info.Add(temp);
                                                }
                                            }
                                            if (!Found)     //item not found in list, add it to list
                                            {
                                                TimeAndPos temp = new TimeAndPos();
                                                temp.XPos = CurrentObj.X;
                                                temp.YPos = CurrentObj.Y;
                                                temp.ZPos = CurrentObj.Z;
                                                temp.RotPos = CurrentObj.Rot;
                                                temp.time = DateTime.Now;       //resolution of 10 milliseconds apparently
                                                ObjArray trackPlayer = new ObjArray();
                                                trackPlayer.GUID = CurrentObj.GUID;
                                                trackPlayer.info.Add(temp);
                                                allObjects.Add(trackPlayer);
                                            }
                                            else
                                            {
                                                Found = false;  //reset value
                                            }
                                            lTest.Text = "Total Players: " + allObjects.Count;
                                        }

                                    }
                                    TrackingBitmap = SketchPlayer(TrackingBitmap, PlayerColor, (Me.X - CurrentObj.X) * TZoom + DisplayWidth / 2, (Me.Y - CurrentObj.Y) * TZoom + DisplayHeight / 2, CurrentObj.Rot, CurrentObj.Name);
                                }//Player alive

                                
                                break;
                        }//end switch
                    }//end if
                    CurrentObj.BaseAddress = wow.ReadUInt(CurrentObj.BaseAddress + (uint)ObjectManagerOff.nextObject);
                }//end while loop of all objects


                this.pName.Text = "Name: " + Me.Name;
                this.pGuid.Text = "GUID: " + Me.GUID;
                this.pX.Text = "X: " + Me.X.ToString();
                this.pY.Text = "Y: " + Me.Y;
                this.pZ.Text = "Z: " + Me.Z;
                this.pRotRad.Text = "Rotation in Rad: " + Math.Round(Me.Rot, 2).ToString() + " R";
                this.pRotDeg.Text = "Rotation in Deg: " + Math.Round(RadToDeg(Me.Rot), 2).ToString() + " D";
                TrackingBitmap = SketchPlayer(TrackingBitmap, LocalPlayer, (Me.X - Me.X) * TZoom + DisplayWidth / 2, (Me.Y - Me.Y) * TZoom + DisplayHeight / 2, Me.Rot, Me.Name);

                if (Target.GUID != 0)
                {
                    this.tName.Text = "Name: " + Target.Name;
                    this.tGuid.Text = "GUID: " + Target.GUID;
                    this.tX.Text = "X: " + Target.X.ToString();
                    this.tY.Text = "Y: " + Target.Y;
                    this.tZ.Text = "Z: " + Target.Z;
                    this.tRotRad.Text = "Rotation in Rad: " + Math.Round(Target.Rot, 2).ToString() + " R";
                    this.tRotDeg.Text = "Rotation in Deg: " + Math.Round(RadToDeg(Target.Rot), 2).ToString() + " D";
                    TrackingBitmap = SketchPlayer(TrackingBitmap, CurrentTarget, (Me.X - Target.X) * TZoom + DisplayWidth / 2, (Me.Y - Target.Y) * TZoom + DisplayHeight / 2, Target.Rot, Target.Name);
                }//end if
                else
                {
                    this.tName.Text = "Name: ";
                    this.tGuid.Text = "GUID: 0";
                    this.tX.Text = "X: ";
                    this.tY.Text = "Y: ";
                    this.tZ.Text = "Z: ";
                    this.tRotRad.Text = "Rotation in Rad: ";
                    this.tRotDeg.Text = "Rotation in Deg: ";

                }//end else
                display.Image = TrackingBitmap;

                lTotObj.Text = "Total Objects: " + TotWowObj;
            }
            catch(Exception)
            {
                return;
            }
        }

        public string NPCNameFromGuid(ulong Guid)
        {
            uint ObjectBase = GetBaseByGuid(Guid);
            if(ObjectBase == 0)
            {
                return "Name not found";
            }
            try
            {
                return wow.ReadASCIIString(wow.ReadUInt(wow.ReadUInt(ObjectBase + (uint)NameOffsets.UnitName1) + (uint)NameOffsets.UnitName2), 30);
            }
            catch(Exception)
            {
                return "Exception";
            }

            //return wow.ReadASCIIString(wow.ReadUInt(wow.ReadUInt(ObjectBase + (uint)NameOffsets.UnitName1) + (uint)NameOffsets.UnitName2), 20);
        }//end MobNameFromGuid


        public uint GetBaseByGuid(ulong Guid)
        {
            TempObj.BaseAddress = FirstObj;     //start from first object

            while (TempObj.BaseAddress != 0)    //loop through all objects till right one is found
            {
                try
                {
                    TempObj.GUID = wow.ReadUInt64(TempObj.BaseAddress + (uint)ObjectOffsets.Guid);
                }
                catch(Exception)
                {
                    TempObj.GUID = 0;
                }
                //TempObj.GUID = wow.ReadUInt64(TempObj.BaseAddress + (uint)ObjectOffsets.Guid, false);

                if(TempObj.GUID == Guid)
                {
                    return TempObj.BaseAddress;
                }
                try
                {
                    TempObj.BaseAddress = wow.ReadUInt(TempObj.BaseAddress + (uint)ObjectManagerOff.nextObject);    //move on to next object
                }
                catch (Exception)
                {
                    return 0;
                }
                
            }
            return 0;   //return 0 if nothing is found.
        }//end GetObjectBaseByGuid


        // Credits WhatSupMang, SillyBoy72 of OwnedCore.com
        public string PlayerNameFromGuid(ulong Guid)
        {
            ulong mask, base_, offset, current, shortGUID, testGUID;

            try
            {
                mask = wow.ReadUInt(WoWBase + (uint)NameOffsets.nameStore + (uint)NameOffsets.nameMask);
            }
            catch (Exception)
            {
                return "Exception";
            }

            base_ = wow.ReadUInt(WoWBase + (uint)NameOffsets.nameStore + (uint)NameOffsets.nameBase);

            shortGUID = Guid & 0xffffffff;
            offset = 12 * (mask & shortGUID);
            try
            {
                current = wow.ReadUInt((uint)base_ + (uint)offset + 8);
            }
            catch (Exception)
            {
                return "Exception";
            }

            offset = wow.ReadUInt((uint)base_ + (uint)offset);

            if ((current & 0x1) == 0x1) { return ""; }

            try
            {
                testGUID = wow.ReadUInt((uint)current);
            }
            catch (Exception)
            {
                return "Exception";
            }


            while (testGUID != shortGUID)
            {
                current = wow.ReadUInt((uint)current + (uint)offset + 4);

                if ((current & 0x1) == 0x1) { return ""; }
                try
                {
                    testGUID = wow.ReadUInt((uint)current);
                }
                catch (Exception)
                {
                    return "Exception";
                }
            }
            return wow.ReadASCIIString((uint)current + (uint)NameOffsets.nameString, 30);
        }//end PlayerNameFromGuid

        private ulong GetGuidByBase(uint Base)
        {
            return wow.ReadUInt64(Base + (uint)ObjectOffsets.Guid);
        }//end GetObjectGuidByBase


        // the rest of the methods are all for drawing the objects on radar

        private Bitmap SketchPlayer(Bitmap img, Color UnitColor, float Ypos, float Xpos, float Rotation, string strName)
        {
            Graphics G = Graphics.FromImage(img);

            G.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.HighQuality;

            //SolidBrush RadarBrush = new SolidBrush(Color.White);
            Pen NormalPen = new Pen(UnitColor, 2F);
            Pen TracingPen = new Pen(UnitColor, 0.1F); //for showing traces

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
                if (!cbShowTrace.Checked)    //how to draw when not tracing
                {
                   /* G.FillEllipse(RadarBrush, Xpos - 5 / 2, Ypos - 5 / 2, 5, 5);
                    G.DrawEllipse(RadarPen, Xpos - 5 / 2, Ypos - 5 / 2, 5, 5);
                    G.DrawLine(RadarPen, Xpos - 5, Ypos + 2, Xpos, Ypos - 8);
                    G.DrawLine(RadarPen, Xpos + 5, Ypos + 2, Xpos, Ypos - 8);
                    G.DrawLine(RadarPen, Xpos, Ypos - 2, Xpos, Ypos - 8);*/

                    G.DrawLine(NormalPen, Xpos - 2, Ypos + 2, Xpos, Ypos - 6);
                    G.DrawLine(NormalPen, Xpos + 2, Ypos + 2, Xpos, Ypos - 6);
                    G.DrawLine(NormalPen, Xpos, Ypos - 2, Xpos, Ypos - 6);
                    G.DrawLine(NormalPen, Xpos - 2, Ypos +1, Xpos + 2, Ypos +1);

                    if(!cbNames.Checked)
                    {
                        WriteText(img, strName, Convert.ToInt32(Xpos) - Convert.ToInt32((strName.Length) * 2.5), Convert.ToInt32(Ypos) + 6);
                    }

                    //DrawText(img, strName, Convert.ToInt32(Xpos) - Convert.ToInt32((strName.Length) * 2.5), Convert.ToInt32(Ypos) + 8);
                }
                else                    //when tracing
                {
                    //G.DrawLine(RadarPen2, Xpos - 2, Ypos + 1, Xpos, Ypos - 3);
                    //G.DrawLine(RadarPen2, Xpos + 2, Ypos + 1, Xpos, Ypos - 3);
                    G.DrawLine(TracingPen, Xpos, Ypos, Xpos, Ypos - 3);
                    //G.DrawLine(RadarPen2, Xpos - 2, Ypos - 3, Xpos + 2, Ypos - 3);
                }
            }
            catch (Exception ex)
            {

            }

            G.Dispose();
            //RadarBrush.Dispose();
            NormalPen.Dispose();
            TracingPen.Dispose();

            return img;

        }//end sketchplayer 


        private Bitmap WriteText(Bitmap img, String sText, int XPos, int YPos)
        {
            Graphics G = Graphics.FromImage(img);
            G.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
            Font DrawFont = new Font("Arial", 7);

            G.DrawString(sText, DrawFont, Brushes.Black, new Point(XPos, YPos));

            G.Dispose();
            DrawFont.Dispose();

            return img;
        }

        private Bitmap WriteTextCenter(Bitmap img, String sText, int size)  // will draw on center of image
        {
            Graphics G = Graphics.FromImage(img);
            G.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.HighQuality;
            Font TextFont = new Font("Arial", size);
            StringFormat SF = new StringFormat();
            SF.LineAlignment = StringAlignment.Center;
            SF.Alignment = StringAlignment.Center;
            RectangleF Rect = new RectangleF(0, 0, DisplayWidth, DisplayHeight);

            G.DrawString(sText, TextFont, Brushes.Black, Rect, SF);

            G.Dispose();
            TextFont.Dispose();
            SF.Dispose();

            return img;
        }

        private void ClearBitmap(ref Bitmap img)
        {
            Graphics G = Graphics.FromImage(img);
            G.Clear(display.BackColor);
            G.Dispose();
        }

        private void bReload_Click(object sender, EventArgs e)
        {
            try
            {
                if (LoadOffsets() == true)
                    TrackerReady = true;

                if (TrackerReady == false)
                {
                    ClearBitmap(ref TrackingBitmap);
                    TrackingBitmap = WriteTextCenter(TrackingBitmap, "Please enter the game world.", 8);
                    display.Image = TrackingBitmap;
                }
            }
            catch (Exception)
            {
                ClearBitmap(ref TrackingBitmap);
                TrackingBitmap = WriteTextCenter(TrackingBitmap, "Could not load offsets. Please Reload.", 8);
                display.Image = TrackingBitmap;
            }  

        }

        private void cbOnTop_CheckedChanged(object sender, EventArgs e)
        {
            if (cbOnTop.Checked)
            {
                WoWdar.ActiveForm.TopMost = true;
            }
            else
            {
                WoWdar.ActiveForm.TopMost = false;
            }
        }

        private void bLog_Click(object sender, EventArgs e)
        {
            Log saveLog = new Log();
            saveLog.WriteToFile(allObjects);
            allObjects.Clear();
        }

        private void cbShowTrace_CheckedChanged(object sender, EventArgs e)
        {
            ClearBitmap(ref TrackingBitmap);
        }

        private void cbNames_CheckedChanged(object sender, EventArgs e)
        {
            ClearBitmap(ref TrackingBitmap);
        }

    }
}
