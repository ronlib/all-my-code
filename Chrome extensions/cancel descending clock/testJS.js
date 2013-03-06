//************************* HIS CODE *************************
var req = createXMLHttpRequest();
function createXMLHttpRequest() {
	var ua;
	if(window.XMLHttpRequest) {
		try {
			ua = new XMLHttpRequest();
		} catch(e) {
			ua = false;
		}
	} else if(window.ActiveXObject) {
		try {
			ua = new ActiveXObject("Microsoft.XMLHTTP");
		} catch(e) {
			ua = false;
		}
	}
	return ua;
}


// תחילת פונקציה 2, זו שכפתור השליחה בטופס קורא לה
function yakir(type,id){
			      var id = id; // הגדרת משתנה שיהיה מחרוזת טקסט
				  var type = type; // הגדרת משתנה שיהיה מחרוזת טקסט
				  var workdir = parseInt(window.location.href.split("/")[3]); // הגדרת משתנה שיהיה מחרוזת טקסט
    req.open('GET', 'divfiles.php?type='+type+'&id='+id+'&workdir='+workdir, true); // שליחת המידע לשרת
		    req.onreadystatechange = function() {
        if (req.readyState == 4) {
			if (type =='down')
			{
				document.forms["example7"].action=req.responseText;
			}
			else if (type =='media')	
			{
			   winStats='toolbar=no,location=no,directories=no,menubar=no,'
			   winStats+='scrollbars=no'
			   if (navigator.appName.indexOf("Microsoft")>=0) {
					winStats+=',left=0,top=0,width=320,height=400'
				}else{
					winStats+=',screenX=0,screenY=0,width=350,height=350'
				}
				var MediaWindow=window.open("http://www.file2host.com/media.php?file="+req.responseText+"&src=7206846866756&scripturl=http://www.file2host.com/","",winStats)     
				MediaWindow.focus()
			}	
			else if (type =='mp3')
			{

			var so = new SWFObject('player.swf','mpl','470','24','9');
			 so.addParam('allowfullscreen','true');
			 so.addParam('allowscriptaccess','always');
			 so.addParam('wmode','opaque');
			 so.addVariable('file',req.responseText);
			 so.addVariable('backcolor','ff7202');
			 so.addVariable('frontcolor','04456f');
			 so.addVariable('lightcolor','04456f');
			 so.addVariable('screencolor','04456f');
			 so.write('mediaspace');
			}	
			}
			}
		req.send(null);
		}
function or()
{
	yakir('down','7');
}
//************************* HIS CODE *************************



or();
downloadButton = document.createElement('button');
downloadButton["innerHTML"] = "הורד ללא לחכות";
downloadButton.setAttribute('type', 'submit');
document.forms["example7"].insertBefore(downloadButton, document.getElementById("dl"));
//document.getElementById("dl").appendChild(downloadButton);


function test() {
	downloadButton = document.createElement('button');
	downloadButton.setAttribute('type', 'submit');
	downloadButton["innerHTML"] = "הורד ללא לחכות";
	// downloadButton.setAttribute('onClick', 'javascript: document.forms["example7"].submit();');
	//document.getElementById("dl4").appendChild(downloadButton);
	//document.forms["example7"].appendChild(downloadButton);
	document.childNodes[1].appendChild(downloadButton);
}

//setTimeout('test()',1000);

chrome.extension.sendRequest({}, function(response) {});