datastore = [
             	{
             		Name: 'Glgltz',
             		Address: 'http://radio.glz.co.il:8000/galgalatz'
             	},
            	{
             		Name: '99fm',
             		Address: 'http://194.90.203.111:8000/99fm'
             	},
         	];



Player = function(){};

Player.prototype.buildPlayerElement = function (mediaUrl)
{
	$player = $('<object>').append(	
			 $('<param>').attr({name:"URL", value: mediaUrl}),
			 $('<param>').attr({name:"uiMode", value:"full"}),
			 $('<param>').attr({name:"mute", value:"false"}),
			 $('<param>').attr({name:"volume", value:"100"}),
			 $('<param>').attr({name:"height", value:"200px"}),
			 $('<param>').attr({name:"showControls", value:"0"}),
			 $('<param>').attr({name:"showStatusBar", value:"0"}),
			 $('<param>').attr({name:"showDisplay", value:"0"}),
			 $('<embed>').attr(
					 {
						 type:"application/x-mplayer2",
						 pluginspage:"http://www.microsoft.com/Windows/MediaPlayer/",
						 src: mediaUrl,
						 name:"Player",
						 width:"100%",
						 height:"30px",
						 autostart:"true",
						 showcontrols:"0",
						 showstatusbar:"0",
						 showdisplay:"0" 
					 })			
			);
	
	return $player;
};

Player.prototype.createListElement = function (datastoreElement)
{
	$listElement = $('<li>');
	$listElement.append($('<div class="name">').text(datastoreElement['Name'])
			.attr('name', datastoreElement['Name']));
	$listElement.append($('<div class="address">').text(datastoreElement['Address'])
			.attr('address', datastoreElement['Address']));
	
	$listElement.click
	(
		this,
		function(player)
		{
			$player = player.data.buildPlayerElement($(this).children('.address').eq(0).attr('address'));
			$("#player").html($player);
		}
	);
	
	return $listElement;
};

Player.prototype.PopulateDisplay = function ()
{	
	$navlist = $('#navlist');
	$navlist.empty();
	
	for (var element in datastore)
		{
			$navlist.append(this.createListElement(datastore[element]));
		}
};

player = new Player();


start = function ()
{
	player.PopulateDisplay();
};

$(document).ready(start);

