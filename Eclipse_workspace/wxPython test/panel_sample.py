import urllib
import wx

class ExamplePanel(wx.Panel):
    def __init__(self, parent):
        wx.Panel.__init__(self, parent)

        # create some sizers
        self.mainSizer = wx.BoxSizer(wx.VERTICAL)        
        self.hSizer2 = wx.BoxSizer(wx.HORIZONTAL)
        self.hSizer1 = wx.BoxSizer(wx.HORIZONTAL)

        self.urlBar = wx.TextCtrl(self, style=wx.TE_LEFT | wx.TE_AUTO_URL)
        getButton = wx.Button(self, label="Get")
        self.Bind(wx.EVT_BUTTON, self.OnClick, getButton)

        self.srcText = wx.TextCtrl(self, style=wx.TE_MULTILINE)

        self.hSizer1.Add(self.urlBar, 1, wx.EXPAND)
        self.hSizer1.Add(getButton)
        self.hSizer2.Add(self.srcText, 1, flag=wx.EXPAND)
        self.mainSizer.Add(self.hSizer1, 0, flag=wx.EXPAND)
        self.mainSizer.Add(self.hSizer2, 1, flag=wx.EXPAND)
        
        self.SetSizer(self.mainSizer)        
        

    def OnClick(self, event):
        urlBarText = self.urlBar.GetValue()
        urlToOpen = urlBarText
        if not urlBarText.lower().startswith("http://"):
            urlToOpen = "http://" + urlToOpen            
        
        self.srcText.SetValue(urllib.urlopen(urlToOpen).read())        

app = wx.App(False)
frame = wx.Frame(None, size=(500,500))
panel = ExamplePanel(frame)
frame.Show()
frame.Fit()
app.MainLoop()
