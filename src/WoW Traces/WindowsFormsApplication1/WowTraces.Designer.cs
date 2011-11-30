namespace WindowsFormsApplication1
{
    partial class WowTraces
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
            this.openFD = new System.Windows.Forms.OpenFileDialog();
            this.iTraces = new System.Windows.Forms.PictureBox();
            this.menuStrip = new System.Windows.Forms.MenuStrip();
            this.fileToolStripMenuItem = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuLoad = new System.Windows.Forms.ToolStripMenuItem();
            this.bTrace = new System.Windows.Forms.Button();
            this.bAllTraces = new System.Windows.Forms.Button();
            this.bClear = new System.Windows.Forms.Button();
            this.gbZoom = new System.Windows.Forms.GroupBox();
            this.rbZoom5 = new System.Windows.Forms.RadioButton();
            this.rbZoom05 = new System.Windows.Forms.RadioButton();
            this.rbZoom4 = new System.Windows.Forms.RadioButton();
            this.rbZoom3 = new System.Windows.Forms.RadioButton();
            this.rbZoom2 = new System.Windows.Forms.RadioButton();
            this.rbZoom1 = new System.Windows.Forms.RadioButton();
            this.gbIcon = new System.Windows.Forms.GroupBox();
            this.rbLine = new System.Windows.Forms.RadioButton();
            this.rbArrow = new System.Windows.Forms.RadioButton();
            this.lXref = new System.Windows.Forms.Label();
            this.lYref = new System.Windows.Forms.Label();
            this.btnSave = new System.Windows.Forms.Button();
            this.nWidth = new System.Windows.Forms.NumericUpDown();
            this.lblInfo = new System.Windows.Forms.Label();
            this.nHeigth = new System.Windows.Forms.NumericUpDown();
            this.lblWidth = new System.Windows.Forms.Label();
            this.lblHeight = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.iTraces)).BeginInit();
            this.menuStrip.SuspendLayout();
            this.gbZoom.SuspendLayout();
            this.gbIcon.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nWidth)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.nHeigth)).BeginInit();
            this.SuspendLayout();
            // 
            // iTraces
            // 
            this.iTraces.Location = new System.Drawing.Point(12, 27);
            this.iTraces.Name = "iTraces";
            this.iTraces.Size = new System.Drawing.Size(917, 917);
            this.iTraces.TabIndex = 0;
            this.iTraces.TabStop = false;
            // 
            // menuStrip
            // 
            this.menuStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.fileToolStripMenuItem});
            this.menuStrip.Location = new System.Drawing.Point(0, 0);
            this.menuStrip.Name = "menuStrip";
            this.menuStrip.Size = new System.Drawing.Size(1061, 24);
            this.menuStrip.TabIndex = 1;
            this.menuStrip.Text = "menuStrip1";
            // 
            // fileToolStripMenuItem
            // 
            this.fileToolStripMenuItem.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuLoad});
            this.fileToolStripMenuItem.Name = "fileToolStripMenuItem";
            this.fileToolStripMenuItem.Size = new System.Drawing.Size(37, 20);
            this.fileToolStripMenuItem.Text = "File";
            // 
            // mnuLoad
            // 
            this.mnuLoad.Name = "mnuLoad";
            this.mnuLoad.Size = new System.Drawing.Size(121, 22);
            this.mnuLoad.Text = "Load File";
            this.mnuLoad.Click += new System.EventHandler(this.mnuLoad_Click);
            // 
            // bTrace
            // 
            this.bTrace.Location = new System.Drawing.Point(935, 27);
            this.bTrace.Name = "bTrace";
            this.bTrace.Size = new System.Drawing.Size(116, 23);
            this.bTrace.TabIndex = 2;
            this.bTrace.Text = "Draw Selected Trace";
            this.bTrace.UseVisualStyleBackColor = true;
            this.bTrace.Click += new System.EventHandler(this.button1_Click);
            // 
            // bAllTraces
            // 
            this.bAllTraces.Location = new System.Drawing.Point(935, 56);
            this.bAllTraces.Name = "bAllTraces";
            this.bAllTraces.Size = new System.Drawing.Size(116, 23);
            this.bAllTraces.TabIndex = 3;
            this.bAllTraces.Text = "Draw All Traces";
            this.bAllTraces.UseVisualStyleBackColor = true;
            this.bAllTraces.Click += new System.EventHandler(this.bAllTraces_Click);
            // 
            // bClear
            // 
            this.bClear.Location = new System.Drawing.Point(935, 85);
            this.bClear.Name = "bClear";
            this.bClear.Size = new System.Drawing.Size(116, 23);
            this.bClear.TabIndex = 4;
            this.bClear.Text = "Clear Traces";
            this.bClear.UseVisualStyleBackColor = true;
            this.bClear.Click += new System.EventHandler(this.bClear_Click);
            // 
            // gbZoom
            // 
            this.gbZoom.Controls.Add(this.rbZoom5);
            this.gbZoom.Controls.Add(this.rbZoom05);
            this.gbZoom.Controls.Add(this.rbZoom4);
            this.gbZoom.Controls.Add(this.rbZoom3);
            this.gbZoom.Controls.Add(this.rbZoom2);
            this.gbZoom.Controls.Add(this.rbZoom1);
            this.gbZoom.Location = new System.Drawing.Point(935, 114);
            this.gbZoom.Name = "gbZoom";
            this.gbZoom.Size = new System.Drawing.Size(116, 162);
            this.gbZoom.TabIndex = 5;
            this.gbZoom.TabStop = false;
            this.gbZoom.Text = "Zoom level";
            // 
            // rbZoom5
            // 
            this.rbZoom5.AutoSize = true;
            this.rbZoom5.Location = new System.Drawing.Point(7, 138);
            this.rbZoom5.Name = "rbZoom5";
            this.rbZoom5.Size = new System.Drawing.Size(38, 17);
            this.rbZoom5.TabIndex = 5;
            this.rbZoom5.TabStop = true;
            this.rbZoom5.Text = "5X";
            this.rbZoom5.UseVisualStyleBackColor = true;
            // 
            // rbZoom05
            // 
            this.rbZoom05.AutoSize = true;
            this.rbZoom05.Location = new System.Drawing.Point(7, 19);
            this.rbZoom05.Name = "rbZoom05";
            this.rbZoom05.Size = new System.Drawing.Size(47, 17);
            this.rbZoom05.TabIndex = 4;
            this.rbZoom05.TabStop = true;
            this.rbZoom05.Text = "0.5X";
            this.rbZoom05.UseVisualStyleBackColor = true;
            // 
            // rbZoom4
            // 
            this.rbZoom4.AutoSize = true;
            this.rbZoom4.Location = new System.Drawing.Point(7, 114);
            this.rbZoom4.Name = "rbZoom4";
            this.rbZoom4.Size = new System.Drawing.Size(38, 17);
            this.rbZoom4.TabIndex = 3;
            this.rbZoom4.Text = "4X";
            this.rbZoom4.UseVisualStyleBackColor = true;
            // 
            // rbZoom3
            // 
            this.rbZoom3.AutoSize = true;
            this.rbZoom3.Location = new System.Drawing.Point(7, 90);
            this.rbZoom3.Name = "rbZoom3";
            this.rbZoom3.Size = new System.Drawing.Size(38, 17);
            this.rbZoom3.TabIndex = 2;
            this.rbZoom3.Text = "3X";
            this.rbZoom3.UseVisualStyleBackColor = true;
            // 
            // rbZoom2
            // 
            this.rbZoom2.AutoSize = true;
            this.rbZoom2.Location = new System.Drawing.Point(7, 66);
            this.rbZoom2.Name = "rbZoom2";
            this.rbZoom2.Size = new System.Drawing.Size(38, 17);
            this.rbZoom2.TabIndex = 1;
            this.rbZoom2.Text = "2X";
            this.rbZoom2.UseVisualStyleBackColor = true;
            // 
            // rbZoom1
            // 
            this.rbZoom1.AutoSize = true;
            this.rbZoom1.Checked = true;
            this.rbZoom1.Location = new System.Drawing.Point(7, 42);
            this.rbZoom1.Name = "rbZoom1";
            this.rbZoom1.Size = new System.Drawing.Size(38, 17);
            this.rbZoom1.TabIndex = 0;
            this.rbZoom1.TabStop = true;
            this.rbZoom1.Text = "1X";
            this.rbZoom1.UseVisualStyleBackColor = true;
            // 
            // gbIcon
            // 
            this.gbIcon.Controls.Add(this.rbLine);
            this.gbIcon.Controls.Add(this.rbArrow);
            this.gbIcon.Location = new System.Drawing.Point(935, 282);
            this.gbIcon.Name = "gbIcon";
            this.gbIcon.Size = new System.Drawing.Size(116, 70);
            this.gbIcon.TabIndex = 6;
            this.gbIcon.TabStop = false;
            this.gbIcon.Text = "Display Icon";
            // 
            // rbLine
            // 
            this.rbLine.AutoSize = true;
            this.rbLine.Location = new System.Drawing.Point(7, 44);
            this.rbLine.Name = "rbLine";
            this.rbLine.Size = new System.Drawing.Size(45, 17);
            this.rbLine.TabIndex = 1;
            this.rbLine.Text = "Line";
            this.rbLine.UseVisualStyleBackColor = true;
            // 
            // rbArrow
            // 
            this.rbArrow.AutoSize = true;
            this.rbArrow.Checked = true;
            this.rbArrow.Location = new System.Drawing.Point(7, 19);
            this.rbArrow.Name = "rbArrow";
            this.rbArrow.Size = new System.Drawing.Size(52, 17);
            this.rbArrow.TabIndex = 0;
            this.rbArrow.TabStop = true;
            this.rbArrow.Text = "Arrow";
            this.rbArrow.UseVisualStyleBackColor = true;
            // 
            // lXref
            // 
            this.lXref.AutoSize = true;
            this.lXref.Location = new System.Drawing.Point(936, 359);
            this.lXref.Name = "lXref";
            this.lXref.Size = new System.Drawing.Size(32, 13);
            this.lXref.TabIndex = 7;
            this.lXref.Text = "Xref :";
            // 
            // lYref
            // 
            this.lYref.AutoSize = true;
            this.lYref.Location = new System.Drawing.Point(936, 376);
            this.lYref.Name = "lYref";
            this.lYref.Size = new System.Drawing.Size(35, 13);
            this.lYref.TabIndex = 8;
            this.lYref.Text = "Yref : ";
            // 
            // btnSave
            // 
            this.btnSave.Location = new System.Drawing.Point(935, 392);
            this.btnSave.Name = "btnSave";
            this.btnSave.Size = new System.Drawing.Size(114, 23);
            this.btnSave.TabIndex = 9;
            this.btnSave.Text = "Save Image";
            this.btnSave.UseVisualStyleBackColor = true;
            this.btnSave.Click += new System.EventHandler(this.btnSave_Click);
            // 
            // nWidth
            // 
            this.nWidth.Location = new System.Drawing.Point(935, 447);
            this.nWidth.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this.nWidth.Name = "nWidth";
            this.nWidth.Size = new System.Drawing.Size(120, 20);
            this.nWidth.TabIndex = 10;
            this.nWidth.Value = new decimal(new int[] {
            917,
            0,
            0,
            0});
            // 
            // lblInfo
            // 
            this.lblInfo.AutoSize = true;
            this.lblInfo.Location = new System.Drawing.Point(935, 418);
            this.lblInfo.Name = "lblInfo";
            this.lblInfo.Size = new System.Drawing.Size(127, 13);
            this.lblInfo.TabIndex = 11;
            this.lblInfo.Text = "Saved Image Dimensions";
            // 
            // nHeigth
            // 
            this.nHeigth.Location = new System.Drawing.Point(935, 490);
            this.nHeigth.Maximum = new decimal(new int[] {
            100000,
            0,
            0,
            0});
            this.nHeigth.Name = "nHeigth";
            this.nHeigth.Size = new System.Drawing.Size(120, 20);
            this.nHeigth.TabIndex = 12;
            this.nHeigth.Value = new decimal(new int[] {
            917,
            0,
            0,
            0});
            // 
            // lblWidth
            // 
            this.lblWidth.AutoSize = true;
            this.lblWidth.Location = new System.Drawing.Point(935, 431);
            this.lblWidth.Name = "lblWidth";
            this.lblWidth.Size = new System.Drawing.Size(38, 13);
            this.lblWidth.TabIndex = 13;
            this.lblWidth.Text = "Width:";
            // 
            // lblHeight
            // 
            this.lblHeight.AutoSize = true;
            this.lblHeight.Location = new System.Drawing.Point(936, 474);
            this.lblHeight.Name = "lblHeight";
            this.lblHeight.Size = new System.Drawing.Size(41, 13);
            this.lblHeight.TabIndex = 14;
            this.lblHeight.Text = "Height:";
            // 
            // WowTraces
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1061, 956);
            this.Controls.Add(this.lblHeight);
            this.Controls.Add(this.lblWidth);
            this.Controls.Add(this.nHeigth);
            this.Controls.Add(this.lblInfo);
            this.Controls.Add(this.nWidth);
            this.Controls.Add(this.btnSave);
            this.Controls.Add(this.lYref);
            this.Controls.Add(this.lXref);
            this.Controls.Add(this.gbIcon);
            this.Controls.Add(this.gbZoom);
            this.Controls.Add(this.bClear);
            this.Controls.Add(this.bAllTraces);
            this.Controls.Add(this.bTrace);
            this.Controls.Add(this.iTraces);
            this.Controls.Add(this.menuStrip);
            this.MainMenuStrip = this.menuStrip;
            this.Name = "WowTraces";
            this.Text = "WoW Traces";
            this.TransparencyKey = System.Drawing.Color.SeaShell;
            ((System.ComponentModel.ISupportInitialize)(this.iTraces)).EndInit();
            this.menuStrip.ResumeLayout(false);
            this.menuStrip.PerformLayout();
            this.gbZoom.ResumeLayout(false);
            this.gbZoom.PerformLayout();
            this.gbIcon.ResumeLayout(false);
            this.gbIcon.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.nWidth)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.nHeigth)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.OpenFileDialog openFD;
        private System.Windows.Forms.PictureBox iTraces;
        private System.Windows.Forms.MenuStrip menuStrip;
        private System.Windows.Forms.ToolStripMenuItem fileToolStripMenuItem;
        private System.Windows.Forms.ToolStripMenuItem mnuLoad;
        private System.Windows.Forms.Button bTrace;
        private System.Windows.Forms.Button bAllTraces;
        private System.Windows.Forms.Button bClear;
        private System.Windows.Forms.GroupBox gbZoom;
        private System.Windows.Forms.RadioButton rbZoom4;
        private System.Windows.Forms.RadioButton rbZoom3;
        private System.Windows.Forms.RadioButton rbZoom2;
        private System.Windows.Forms.RadioButton rbZoom1;
        private System.Windows.Forms.GroupBox gbIcon;
        private System.Windows.Forms.RadioButton rbLine;
        private System.Windows.Forms.RadioButton rbArrow;
        private System.Windows.Forms.Label lXref;
        private System.Windows.Forms.Label lYref;
        private System.Windows.Forms.RadioButton rbZoom05;
        private System.Windows.Forms.Button btnSave;
        private System.Windows.Forms.NumericUpDown nWidth;
        private System.Windows.Forms.Label lblInfo;
        private System.Windows.Forms.NumericUpDown nHeigth;
        private System.Windows.Forms.Label lblWidth;
        private System.Windows.Forms.Label lblHeight;
        private System.Windows.Forms.RadioButton rbZoom5;
    }
}

