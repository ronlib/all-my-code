$(document).ready(function() {
//		$('a').click( function (event) {
//				alert('Thanks for visiting!');
//				event.preventDefault();
//				$(this).hide("slow");
//				}
//			);

		var $calculator = $("div[name='calculator']:first");
		var $calc_table = $("<table></table>");
		$calculator.append($calc_table);
		$calc_table.append(
			$("<tr/>").attr("id", "row1"),
			$("<tr/>").attr("id", "row2"),
			$("<tr/>").attr("id", "row3"),
			$("<tr/>").attr("id", "row4")
		);
		
		$("#row1").append(
				$("<button/>").attr({value: "1", class: "calcbutton"}).text("1"),
				$("<button/>").attr({value: "2", class: "calcbutton"}).text("2"),
				$("<button/>").attr({value: "3", class: "calcbutton"}).text("3")
				);
		$("#row2").append(
				$("<button/>").attr({value: "4", class: "calcbutton"}).text("4"),
				$("<button/>").attr({value: "5", class: "calcbutton"}).text("5"),
				$("<button/>").attr({value: "6", class: "calcbutton"}).text("6")
				);
		$("#row3").append(
				$("<button/>").attr({value: "7", class: "calcbutton"}).text("7"),
				$("<button/>").attr({value: "8", class: "calcbutton"}).text("8"),
				$("<button/>").attr({value: "9", class: "calcbutton"}).text("9")
				);
		$("#row4").append(
				$("<button/>").attr({value: "+", class: "calcbutton"}).text("+"),
				$("<button/>").attr({value: "0", class: "calcbutton"}).text("0"),
				$("<button/>").attr({value: "-", class: "calcbutton"}).text("-")
				);
		
		$("button").click(handle_click);
		
		
		}
		
	);