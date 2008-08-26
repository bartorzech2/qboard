/**

This plugin provides basic "dice rolling" features, intended
for use in games/gaming utilities based on jQuery.

Usage:

	<button id='MyDieButton'></button>

	$('#MyDieButton').bogoDice({ ... options ... });

Results are undefined if this plugin is applied to a non-button
UI element, but in theory it "should" work with any element
for which jQuery(...).click() behaves properly.

Options are:

	- sides [integer, default=6], the number of sides the dice have.
	Results are undefined if this is not a positive integer.

	- count [integer, default=1], the number of dice to roll.

	- low [integer, default=1], the lowest value of the die. This is
	only for special-case use, like a die which should roll -1..+1
	(e.g. Fudge dice). The sides parameter is used to determine the
	high value.

	- resultsTarget [Mixed, default=null], a target object to send the
	dice results to. If it is a function, it is called and passed
	the total of a dice roll result (as a string) and an array containing
	each individual roll. If it is not a function then it is assumed to
	be a jQuery selector, and $(resultsTarget).text(...) will be called
	and passed the die results.

	- verbose [Boolean, default=true], if false then only the
	grand total of a die result is sent to the resultsTarget,
	otherwise an "informative" form is used. e.g. rolling 3d6
	will have a result in the form of "3 + 2 + 5 = 10".

	- label [string, default=count+'d'+sides], a label string to
	apply to the target button.

///////////////////////////////////////////////////////////////////////
Example usage:

	<span id='DieResults'>Click a die button</span>
	<button id='Button1d6'></button>
	<button id='Button5d6'></button>
...
	$('#Button1d6').bogoDice({
		resultsTarget:'#DieResults'
	});

	$('#Button5d6').bogoDice({
		count:5,
		label:'5 dice',
		resultsTarget:'#DieResults'
	});


	We can support Fudge (www.fudgerpg.com) dice like this:

	<button id='Button4dF'></button>

	$('#Button4dF').bogoDice({
		count:4,
		sides:3,
		low:-1,
		label:'4dF',
		verbose:false,
		resultsTarget:function(total,accum){
			$('#DiceResults').text('4dF ['+accum.join(', ')+"] = "+total);
		}
	});
///////////////////////////////////////////////////////////////////////
Potential TODOs:

- Ability to fetch results using Ajax, to allow server-produced
results.

- Add a modifier to the roll total (e.g. 3d6+3 or 1d6-1).

- Somehow support special types of dice, like dF (Fudge dice).

///////////////////////////////////////////////////////////////////////
Author:

	http://wanderinghorse.net/home/stephan/

Plugin home page:

	http://wanderinghorse.net/computing/javascript/jquery/bogodice/

License: Public Domain


Revision history:

- 20080825:
        - Added "Roll #xyz" prefix to the output, to avoid confusion when two
	successive rolls have the same result.

- 20070815:
	- added .low=integer option to support Fudge dice.
	- resultsTarget callback function now passed array of results.
	- Corrected completely bogus examples.
	- Changed .count default from 3 to 1.

- 20070805: initial release


*/
jQuery.fn.bogoDice = function( props ) {
	props = jQuery.extend({
		sides:6,
		count:1,
		low:1,
		resultsTarget:null,
		verbose:true,
		rollCount:0,
		},
		props ? props : []);
	props = jQuery.extend({
		label:props.count+'d'+props.sides
		},
		props);

	function rollDie(low,sides,count,target) {
		if( ! count ) count = 1;
		++props.rollCount;
		var accum = target ? new Array() : null;
		var total = 0;
		var res =0;
		for( var i = 0; i < count; ++i ) {
			res = Math.floor(Math.random()*sides+low);
			total += res;
			if( accum ) accum[i] = res;
		}
		if( target ) {
		    var prefix = 'Roll #'+props.rollCount+': ';
			var msg = prefix+total;
			if( props.verbose ) {
				msg = prefix + props.label+': ';
				if( count == 1 ) { msg += total; }
				else {
            		msg += (accum.join(' + '))+' = '+total;
				}
			}
			if( 'function' == typeof target ) {
				target( msg,accum );
			} else {
				$(target).text( msg,accum );
			}
		}
		return total;
	};

	this.text(props.label);
	this.click(function(){
			rollDie( props.low, props.sides, props.count, props.resultsTarget );
		});
	return this;
};
