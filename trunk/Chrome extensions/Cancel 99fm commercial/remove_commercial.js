if (document.location.href.match(/radio=7[#]?$/) != null) {
	document.getElementById("Player").innerHTML = "<object id=\"Player\" height=\"240\" width=\"320\" classid=\"CLSID:6BF52A52-394A-11d3-B153-00C04F79FAA6\">\
		<param name=\"URL\" value=\"http://radio.coolsite.co.il/99.asx\">\
		<param name=\"uiMode\" value=\"none\">\
		<param name=\"mute\" value=\"false\">\
		<param name=\"volume\" value=\"100\">\
		<param name=\"ShowControls\" value=\"0\">\
		<param name=\"ShowStatusBar\" value=\"0\">\
		<param name=\"ShowDisplay\" value=\"0\">\
		<embed type=\"application/x-mplayer2\" pluginspage=\"http://www.microsoft.com/Windows/MediaPlayer/\" src=\"http://radio.coolsite.co.il/99.asx\" name=\"Player\" width=\"320\" height=\"240\" autostart=\"true\" showcontrols=\"0\" showstatusbar=\"0\" showdisplay=\"0\">\
	</object>";
}