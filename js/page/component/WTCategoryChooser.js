var WTCategoryChooser = function(title, util, api ) {
	this.title = title;
	this.util = util;
	this.api = api;
	this.selectBox = null;
	this.parentItem = null;
	this.fetched = false;
	this.fetchCategories();
};

WTCategoryChooser.prototype.fetchCategories = function() {
	var me = this;
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
		me.fetched = true;
		me._display();
	});	
};

WTCategoryChooser.prototype._display = function() {
	if(!this.fetched || this.parentItem == null) 
		return;

	if(wtrights["edit-page-metadata"]) {
		if(wtpagenotfound && !wtrights['createpage'])
			return;

		this.selectBox = $('<select multiple data-placeholder="Choose a Category" class="category-select" style="width:100%"></select>');
		for(var i=0; i<wtallcategories.length; i++) {
			var cat = wtallcategories[i];
			var option = $('<option>'+cat+'</option>');
			if(wtcategories[cat])
				option.attr('selected', 'selected');
			this.selectBox.append(option);
		}

		var header = $('<div class="heading"></div>').append($('<b>Category</b>'));
		this.parentItem.append(header);
		var wrapper = $('<div style="padding:5px"></div>');
		var txt = "Set Categories for this page";
		if(wtpagenotfound) 
			txt = "Create the page by choosing Categories for this page";
		wrapper.append(txt).append("<br />");
		wrapper.append(this.selectBox);
		this.parentItem.append(wrapper);

		this.selectBox.select2({tags: true});

		var me = this;
		this.selectBox.on('change', function(e, params) {
			var values = me.selectBox.val();
			me.parentItem.mask(lpMsg('Setting Categories..'));
			me.api.createPageWithCategories( me.title, values, function(response) {
				if(!response || !response.wtfacts) return; 
				if(response.wtfacts.result == 'Success') {
					window.location.reload();
				}
			});
		});
	}
};

WTCategoryChooser.prototype.display = function( item ) {
	var me = this;
	this.parentItem = item;
	this._display();
};

