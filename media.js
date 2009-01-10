// $Id$

/**
 * @file 
 * This file handles the JS for Media Module functions
 */
 
 
 $(document).ready( function () { 
 
  // hide the display information on page load
  $('.media.browser.display').each(function() {
    $(this).hide();
  });
 
   // Activate drawers and displays when the page loads
  $('.ui-tabs-panel').each(function () { 
	  // set the first drawer active
	  $(this).children('.media.browser.drawer:first').addClass('active');
	  // show the display of the first drawer
	  $(this).children('.media.browser.display:first').addClass('active').show();
	});
	
	// now we need to bind click functionality on drawers to display
	$('.media.browser.drawer').click( function () {
	  // constrain the click to inside this tab
	  
	  // get parent
	  var parent = $(this).parent('div');
	  // remove active class from current drawer
	  $(parent).children('.media.browser.drawer.active').removeClass('active');
	  // remove active class from current display
	  $(parent).children('.media.browser.display.active').removeClass('active').hide();
	  // set this drawer active
	  $(this).addClass('active');
	  // make the display for this drawer active
	  $(this).next('.media.browser.display').addClass('active').show();
	});


 });
 