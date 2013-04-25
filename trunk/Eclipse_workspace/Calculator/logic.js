function handle_click(eventObject)
{
	buttonClicked = $(this).attr("value");
	$("#status").text(buttonClicked);
}