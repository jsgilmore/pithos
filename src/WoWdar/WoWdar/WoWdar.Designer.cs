namespace WoWdar
{
    partial class WoWdar
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.display = new System.Windows.Forms.PictureBox();
            this.lPlayerinfo = new System.Windows.Forms.Label();
            this.pGuid = new System.Windows.Forms.Label();
            this.pName = new System.Windows.Forms.Label();
            this.pX = new System.Windows.Forms.Label();
            this.pY = new System.Windows.Forms.Label();
            this.pZ = new System.Windows.Forms.Label();
            this.lTarget = new System.Windows.Forms.Label();
            this.tGuid = new System.Windows.Forms.Label();
            this.tName = new System.Windows.Forms.Label();
            this.tX = new System.Windows.Forms.Label();
            this.tY = new System.Windows.Forms.Label();
            this.tZ = new System.Windows.Forms.Label();
            this.RadarTimer = new System.Windows.Forms.Timer(this.components);
            this.pRotRad = new System.Windows.Forms.Label();
            this.pRotDeg = new System.Windows.Forms.Label();
            this.tRotRad = new System.Windows.Forms.Label();
            this.tRotDeg = new System.Windows.Forms.Label();
            this.lTotObj = new System.Windows.Forms.Label();
            this.bReload = new System.Windows.Forms.Button();
            this.gbZoom = new System.Windows.Forms.GroupBox();
            this.rZoom4 = new System.Windows.Forms.RadioButton();
            this.rZoom3 = new System.Windows.Forms.RadioButton();
            this.rZoom2 = new System.Windows.Forms.RadioButton();
            this.rZoom1 = new System.Windows.Forms.RadioButton();
            this.cbOnTop = new System.Windows.Forms.CheckBox();
            this.cbTrack = new System.Windows.Forms.CheckBox();
            this.bLog = new System.Windows.Forms.Button();
            this.lTest = new System.Windows.Forms.Label();
            this.cbShowTrace = new System.Windows.Forms.CheckBox();
            this.cbNames = new System.Windows.Forms.CheckBox();
            ((System.ComponentModel.ISupportInitialize)(this.display)).BeginInit();
            this.gbZoom.SuspendLayout();
            this.SuspendLayout();
            // 
            // display
            // 
            this.display.Location = new System.Drawing.Point(12, 12);
            this.display.Name = "display";
            this.display.Size = new System.Drawing.Size(552, 552);
            this.display.TabIndex = 0;
            this.display.TabStop = false;
            // 
            // lPlayerinfo
            // 
            this.lPlayerinfo.AutoSize = true;
            this.lPlayerinfo.Location = new System.Drawing.Point(570, 12);
            this.lPlayerinfo.Name = "lPlayerinfo";
            this.lPlayerinfo.Size = new System.Drawing.Size(60, 13);
            this.lPlayerinfo.TabIndex = 1;
            this.lPlayerinfo.Text = "Player Info:";
            // 
            // pGuid
            // 
            this.pGuid.AutoSize = true;
            this.pGuid.Location = new System.Drawing.Point(570, 25);
            this.pGuid.Name = "pGuid";
            this.pGuid.Size = new System.Drawing.Size(37, 13);
            this.pGuid.TabIndex = 2;
            this.pGuid.Text = "GUID:";
            // 
            // pName
            // 
            this.pName.AutoSize = true;
            this.pName.Location = new System.Drawing.Point(570, 38);
            this.pName.Name = "pName";
            this.pName.Size = new System.Drawing.Size(35, 13);
            this.pName.TabIndex = 3;
            this.pName.Text = "Name";
            // 
            // pX
            // 
            this.pX.AutoSize = true;
            this.pX.Location = new System.Drawing.Point(570, 51);
            this.pX.Name = "pX";
            this.pX.Size = new System.Drawing.Size(17, 13);
            this.pX.TabIndex = 4;
            this.pX.Text = "X:";
            // 
            // pY
            // 
            this.pY.AutoSize = true;
            this.pY.Location = new System.Drawing.Point(570, 64);
            this.pY.Name = "pY";
            this.pY.Size = new System.Drawing.Size(17, 13);
            this.pY.TabIndex = 5;
            this.pY.Text = "Y:";
            // 
            // pZ
            // 
            this.pZ.AutoSize = true;
            this.pZ.Location = new System.Drawing.Point(570, 77);
            this.pZ.Name = "pZ";
            this.pZ.Size = new System.Drawing.Size(17, 13);
            this.pZ.TabIndex = 6;
            this.pZ.Text = "Z:";
            // 
            // lTarget
            // 
            this.lTarget.AutoSize = true;
            this.lTarget.Location = new System.Drawing.Point(570, 129);
            this.lTarget.Name = "lTarget";
            this.lTarget.Size = new System.Drawing.Size(62, 13);
            this.lTarget.TabIndex = 7;
            this.lTarget.Text = "Target Info:";
            // 
            // tGuid
            // 
            this.tGuid.AutoSize = true;
            this.tGuid.Location = new System.Drawing.Point(570, 142);
            this.tGuid.Name = "tGuid";
            this.tGuid.Size = new System.Drawing.Size(37, 13);
            this.tGuid.TabIndex = 8;
            this.tGuid.Text = "GUID:";
            // 
            // tName
            // 
            this.tName.AutoSize = true;
            this.tName.Location = new System.Drawing.Point(570, 155);
            this.tName.Name = "tName";
            this.tName.Size = new System.Drawing.Size(35, 13);
            this.tName.TabIndex = 9;
            this.tName.Text = "Name";
            // 
            // tX
            // 
            this.tX.AutoSize = true;
            this.tX.Location = new System.Drawing.Point(570, 168);
            this.tX.Name = "tX";
            this.tX.Size = new System.Drawing.Size(17, 13);
            this.tX.TabIndex = 10;
            this.tX.Text = "X:";
            // 
            // tY
            // 
            this.tY.AutoSize = true;
            this.tY.Location = new System.Drawing.Point(570, 181);
            this.tY.Name = "tY";
            this.tY.Size = new System.Drawing.Size(17, 13);
            this.tY.TabIndex = 11;
            this.tY.Text = "Y:";
            // 
            // tZ
            // 
            this.tZ.AutoSize = true;
            this.tZ.Location = new System.Drawing.Point(570, 194);
            this.tZ.Name = "tZ";
            this.tZ.Size = new System.Drawing.Size(17, 13);
            this.tZ.TabIndex = 12;
            this.tZ.Text = "Z:";
            // 
            // RadarTimer
            // 
            this.RadarTimer.Enabled = true;
            this.RadarTimer.Interval = 15;
            this.RadarTimer.Tick += new System.EventHandler(this.RadarTimer_Tick);
            // 
            // pRotRad
            // 
            this.pRotRad.AutoSize = true;
            this.pRotRad.Location = new System.Drawing.Point(570, 90);
            this.pRotRad.Name = "pRotRad";
            this.pRotRad.Size = new System.Drawing.Size(84, 13);
            this.pRotRad.TabIndex = 13;
            this.pRotRad.Text = "Rotation in Rad:";
            // 
            // pRotDeg
            // 
            this.pRotDeg.AutoSize = true;
            this.pRotDeg.Location = new System.Drawing.Point(570, 103);
            this.pRotDeg.Name = "pRotDeg";
            this.pRotDeg.Size = new System.Drawing.Size(107, 13);
            this.pRotDeg.TabIndex = 14;
            this.pRotDeg.Text = "Rotation in Degrees: ";
            // 
            // tRotRad
            // 
            this.tRotRad.AutoSize = true;
            this.tRotRad.Location = new System.Drawing.Point(570, 207);
            this.tRotRad.Name = "tRotRad";
            this.tRotRad.Size = new System.Drawing.Size(84, 13);
            this.tRotRad.TabIndex = 15;
            this.tRotRad.Text = "Rotation in Rad:";
            // 
            // tRotDeg
            // 
            this.tRotDeg.AutoSize = true;
            this.tRotDeg.Location = new System.Drawing.Point(570, 220);
            this.tRotDeg.Name = "tRotDeg";
            this.tRotDeg.Size = new System.Drawing.Size(104, 13);
            this.tRotDeg.TabIndex = 16;
            this.tRotDeg.Text = "Rotation in Degrees:";
            // 
            // lTotObj
            // 
            this.lTotObj.AutoSize = true;
            this.lTotObj.Location = new System.Drawing.Point(570, 283);
            this.lTotObj.Name = "lTotObj";
            this.lTotObj.Size = new System.Drawing.Size(73, 13);
            this.lTotObj.TabIndex = 17;
            this.lTotObj.Text = "Total Objects:";
            // 
            // bReload
            // 
            this.bReload.Location = new System.Drawing.Point(573, 422);
            this.bReload.Name = "bReload";
            this.bReload.Size = new System.Drawing.Size(75, 23);
            this.bReload.TabIndex = 19;
            this.bReload.Text = "Reload";
            this.bReload.UseVisualStyleBackColor = true;
            this.bReload.Click += new System.EventHandler(this.bReload_Click);
            // 
            // gbZoom
            // 
            this.gbZoom.Controls.Add(this.rZoom4);
            this.gbZoom.Controls.Add(this.rZoom3);
            this.gbZoom.Controls.Add(this.rZoom2);
            this.gbZoom.Controls.Add(this.rZoom1);
            this.gbZoom.Location = new System.Drawing.Point(585, 451);
            this.gbZoom.Name = "gbZoom";
            this.gbZoom.Size = new System.Drawing.Size(63, 114);
            this.gbZoom.TabIndex = 20;
            this.gbZoom.TabStop = false;
            this.gbZoom.Text = "Zoom";
            // 
            // rZoom4
            // 
            this.rZoom4.AutoSize = true;
            this.rZoom4.Location = new System.Drawing.Point(7, 88);
            this.rZoom4.Name = "rZoom4";
            this.rZoom4.Size = new System.Drawing.Size(36, 17);
            this.rZoom4.TabIndex = 3;
            this.rZoom4.Text = "4x";
            this.rZoom4.UseVisualStyleBackColor = true;
            // 
            // rZoom3
            // 
            this.rZoom3.AutoSize = true;
            this.rZoom3.Location = new System.Drawing.Point(7, 65);
            this.rZoom3.Name = "rZoom3";
            this.rZoom3.Size = new System.Drawing.Size(36, 17);
            this.rZoom3.TabIndex = 2;
            this.rZoom3.Text = "3x";
            this.rZoom3.UseVisualStyleBackColor = true;
            // 
            // rZoom2
            // 
            this.rZoom2.AutoSize = true;
            this.rZoom2.Location = new System.Drawing.Point(7, 42);
            this.rZoom2.Name = "rZoom2";
            this.rZoom2.Size = new System.Drawing.Size(36, 17);
            this.rZoom2.TabIndex = 1;
            this.rZoom2.Text = "2x";
            this.rZoom2.UseVisualStyleBackColor = true;
            // 
            // rZoom1
            // 
            this.rZoom1.AutoSize = true;
            this.rZoom1.Checked = true;
            this.rZoom1.Location = new System.Drawing.Point(7, 19);
            this.rZoom1.Name = "rZoom1";
            this.rZoom1.Size = new System.Drawing.Size(36, 17);
            this.rZoom1.TabIndex = 0;
            this.rZoom1.TabStop = true;
            this.rZoom1.Text = "1x";
            this.rZoom1.UseVisualStyleBackColor = true;
            // 
            // cbOnTop
            // 
            this.cbOnTop.AutoSize = true;
            this.cbOnTop.Location = new System.Drawing.Point(573, 347);
            this.cbOnTop.Name = "cbOnTop";
            this.cbOnTop.Size = new System.Drawing.Size(98, 17);
            this.cbOnTop.TabIndex = 21;
            this.cbOnTop.Text = "Always On Top";
            this.cbOnTop.UseVisualStyleBackColor = true;
            this.cbOnTop.CheckedChanged += new System.EventHandler(this.cbOnTop_CheckedChanged);
            // 
            // cbTrack
            // 
            this.cbTrack.AutoSize = true;
            this.cbTrack.Location = new System.Drawing.Point(573, 370);
            this.cbTrack.Name = "cbTrack";
            this.cbTrack.Size = new System.Drawing.Size(91, 17);
            this.cbTrack.TabIndex = 22;
            this.cbTrack.Text = "Track Players";
            this.cbTrack.UseVisualStyleBackColor = true;
            // 
            // bLog
            // 
            this.bLog.Location = new System.Drawing.Point(573, 393);
            this.bLog.Name = "bLog";
            this.bLog.Size = new System.Drawing.Size(75, 23);
            this.bLog.TabIndex = 23;
            this.bLog.Text = "Create Log";
            this.bLog.UseVisualStyleBackColor = true;
            this.bLog.Click += new System.EventHandler(this.bLog_Click);
            // 
            // lTest
            // 
            this.lTest.AutoSize = true;
            this.lTest.Location = new System.Drawing.Point(570, 270);
            this.lTest.Name = "lTest";
            this.lTest.Size = new System.Drawing.Size(71, 13);
            this.lTest.TabIndex = 24;
            this.lTest.Text = "Total Players:";
            // 
            // cbShowTrace
            // 
            this.cbShowTrace.AutoSize = true;
            this.cbShowTrace.Location = new System.Drawing.Point(573, 324);
            this.cbShowTrace.Name = "cbShowTrace";
            this.cbShowTrace.Size = new System.Drawing.Size(121, 17);
            this.cbShowTrace.TabIndex = 27;
            this.cbShowTrace.Text = "Show Player Traces";
            this.cbShowTrace.UseVisualStyleBackColor = true;
            this.cbShowTrace.CheckedChanged += new System.EventHandler(this.cbShowTrace_CheckedChanged);
            // 
            // cbNames
            // 
            this.cbNames.AutoSize = true;
            this.cbNames.Location = new System.Drawing.Point(573, 301);
            this.cbNames.Name = "cbNames";
            this.cbNames.Size = new System.Drawing.Size(84, 17);
            this.cbNames.TabIndex = 28;
            this.cbNames.Text = "Hide Names";
            this.cbNames.UseVisualStyleBackColor = true;
            this.cbNames.CheckedChanged += new System.EventHandler(this.cbNames_CheckedChanged);
            // 
            // WoWdar
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(745, 575);
            this.Controls.Add(this.cbNames);
            this.Controls.Add(this.cbShowTrace);
            this.Controls.Add(this.lTest);
            this.Controls.Add(this.bLog);
            this.Controls.Add(this.cbTrack);
            this.Controls.Add(this.cbOnTop);
            this.Controls.Add(this.gbZoom);
            this.Controls.Add(this.bReload);
            this.Controls.Add(this.lTotObj);
            this.Controls.Add(this.tRotDeg);
            this.Controls.Add(this.tRotRad);
            this.Controls.Add(this.pRotDeg);
            this.Controls.Add(this.pRotRad);
            this.Controls.Add(this.tZ);
            this.Controls.Add(this.tY);
            this.Controls.Add(this.tX);
            this.Controls.Add(this.tName);
            this.Controls.Add(this.tGuid);
            this.Controls.Add(this.lTarget);
            this.Controls.Add(this.pZ);
            this.Controls.Add(this.pY);
            this.Controls.Add(this.pX);
            this.Controls.Add(this.pName);
            this.Controls.Add(this.pGuid);
            this.Controls.Add(this.lPlayerinfo);
            this.Controls.Add(this.display);
            this.Name = "WoWdar";
            this.Text = "WoWdar Tracker";
            this.Load += new System.EventHandler(this.WoWdar_Load);
            ((System.ComponentModel.ISupportInitialize)(this.display)).EndInit();
            this.gbZoom.ResumeLayout(false);
            this.gbZoom.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.PictureBox display;
        private System.Windows.Forms.Label lPlayerinfo;
        private System.Windows.Forms.Label pGuid;
        private System.Windows.Forms.Label pName;
        private System.Windows.Forms.Label pX;
        private System.Windows.Forms.Label pY;
        private System.Windows.Forms.Label pZ;
        private System.Windows.Forms.Label lTarget;
        private System.Windows.Forms.Label tGuid;
        private System.Windows.Forms.Label tName;
        private System.Windows.Forms.Label tX;
        private System.Windows.Forms.Label tY;
        private System.Windows.Forms.Label tZ;
        private System.Windows.Forms.Timer RadarTimer;
        private System.Windows.Forms.Label pRotRad;
        private System.Windows.Forms.Label pRotDeg;
        private System.Windows.Forms.Label tRotRad;
        private System.Windows.Forms.Label tRotDeg;
        private System.Windows.Forms.Label lTotObj;
        private System.Windows.Forms.Button bReload;
        private System.Windows.Forms.GroupBox gbZoom;
        private System.Windows.Forms.RadioButton rZoom4;
        private System.Windows.Forms.RadioButton rZoom3;
        private System.Windows.Forms.RadioButton rZoom2;
        private System.Windows.Forms.RadioButton rZoom1;
        private System.Windows.Forms.CheckBox cbOnTop;
        private System.Windows.Forms.CheckBox cbTrack;
        private System.Windows.Forms.Button bLog;
        private System.Windows.Forms.Label lTest;
        private System.Windows.Forms.CheckBox cbShowTrace;
        private System.Windows.Forms.CheckBox cbNames;
    }
}

