var WTCategoryChooser = function(title, util, api ) {
	this.title = title;
	this.util = util;
	this.api = api;
	this.fetchCategories();
};

WTCategoryChooser.prototype.fetchCategories = function() {
	var catmap = new Array();
	for(var i=0; i<wtallcategories.length; i++)
		catmap[wtallcategories[i]] = true;

	this.api.getAllCategories( function (cats) {
		for(var i=0; i<cats.length; i++) {
			var cat = cats[i];
			if(!catmap[cat]) {
				catmap[cat] = true;
				wtallcategories.push(cat);
			}
		}
		wtallcategories.sort();
	});	
};

WTCategoryChooser.prototype.display = function( item ) {
	var me = this;

	var curcat = '';
	$.each(Object.keys(wtcategories), function(i, cat) {
		curcat = cat;
	});

	$select = $('<input type="text" placeholder="Enter a Category (down arrow to see all)" style="width:50%"/>');
	$select.val(curcat);
	$select.autocomplete({
		delay:300,
		minLength:0,
		highlightClass:'none',
		source: wtallcategories
	});

	$select.keyup(function( e ) {
		if(e.keyCode == 13){
			var val = $select.val();
			if(curcat == val) return;
			item.mask(lpMsg('Setting Category..'));
			me.api.createPageWithCategory( me.title, val, function(response) {
				if(!response || !response.wtfacts) return; 
				if(response.wtfacts.result == 'Success') {
					window.location.reload();
				}
			});
		}
	});

	if(wtuid) {
		var header = $('<div class="heading"></div>').append($('<b>Category</b>'));
		item.append(header);
		var wrapper = $('<div style="padding:5px"></div>');
		var txt = "Set Category for this page (will reset contents)";
		if(wtpagenotfound) 
			txt = "Create the page by choosing a Category for this page";
		else if(curcat)
			var txt = "Change Category for this page (will reset contents)";
		var catdiv = $('<div>'+txt+':</div>');
		catdiv.append('<ul></ul>').append($select);
		wrapper.append(catdiv);
		item.append(wrapper);
	}
};

