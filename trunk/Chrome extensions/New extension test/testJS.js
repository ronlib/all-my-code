//alert("Success in changing the page!");
//document.body.style.backgroundColor='red';
//alert("Success11!");
//	document.body.style.backgroundColor='red'
// $(document).ready(function() {
				// chrome.tabs.executeScript(null,
					// {code:"document.body.style.backgroundColor='red'"});
				// document.body.style.backgroundColor='red'
				// alert("Success!");
			// });
			
			
			
var headID = document.getElementsByTagName("head")[0];
var newScript = document.createElement('script');
newScript.type = 'text/javascript';
newScript.src = '$(document).ready(function() {
				document.body.style.backgroundColor="red"
				alert("Success!");
			});';
headID.appendChild(newScript);