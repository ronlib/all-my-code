start = function() {
	commandPattern = /\bwindow.location=".*"/i;
	urlChangeCmd = commandPattern.exec($('body').html());	
	eval(urlChangeCmd[0]);
};

$(document).ready(start);