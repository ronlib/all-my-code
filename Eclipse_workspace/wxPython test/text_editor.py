import wx
import os

class MyFrame(wx.Frame):
    def __init__(self, parent, title):
        super(MyFrame, self).__init__(parent, title=title, size=(200, 100))
        self.control = wx.TextCtrl(self, style=wx.TE_MULTILINE)
        self.CreateStatusBar()
        
        filemenu = wx.Menu()
        
        menuOpen = filemenu.Append(wx.ID_OPEN, "&Open", "Opens a file to edit")
        filemenu.AppendSeparator()
        menuAbout = filemenu.Append(wx.ID_ABOUT, "&About", "Information about the program")
        filemenu.AppendSeparator()
        menuExit = filemenu.Append(wx.ID_EXIT, "E&xit", "Terminate the program")
        
        menuBar = wx.MenuBar()
        menuBar.Append(filemenu, "&File")        
        self.SetMenuBar(menuBar)
        
        self.Bind(wx.EVT_MENU, self.OnOpen, menuOpen)
        self.Bind(wx.EVT_MENU, self.OnAbout, menuAbout)
        self.Bind(wx.EVT_MENU, self.OnExit, menuExit)
        
        self.sizer2 = wx.BoxSizer(wx.HORIZONTAL)
        self.buttons = []
        for i in range (6):
            self.buttons.append(wx.Button(self, -1, "Button &%d" % (i,)))
            self.sizer2.Add(self.buttons[i], 1, wx.EXPAND)
            
        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.control, 1, wx.EXPAND)
        self.sizer.Add(self.sizer2, 0, wx.EXPAND)
        
        self.SetSizer(self.sizer)
        # Called implicitly by self.SetSizer        
        self.SetAutoLayout(1)
        self.sizer.Fit(self)
        self.Show()
        
    def OnOpen(self, event):
        """Open a file"""
        self.dirname = ''
        dlg = wx.FileDialog(self, "Choose a file", self.dirname)
        if dlg.ShowModal() == wx.ID_OK:
            filename = dlg.GetFilename()
            self.dirname = dlg.GetDirectory()
            try:
                filePath = os.path.join(self.dirname, filename)                
                with open(filePath, 'r') as openFile:
                    self.control.SetValue(openFile.read())
            except:
                self.GetStatusBar().SetStatusText('Unable to open file "%s"' % 
                                                  (filePath,))
        dlg.Destroy()
    
    def OnAbout(self, event):
        dlg = wx.MessageDialog(self, "A small text editor", "About Sample " 
                                     "Editor", wx.OK)        
        dlg.ShowModal()
        dlg.Destroy()
        
    def OnExit(self, event):
        self.Close()    
        
def run():
    app = wx.App(False)
    frame = MyFrame(None, 'Small editor')
    app.MainLoop()
    
if __name__ == '__main__':
    run()
