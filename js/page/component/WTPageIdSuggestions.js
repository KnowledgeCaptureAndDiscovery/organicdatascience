var WTPageIdSuggestions = function(title, names, util, api ) {
	this.title = title;
  this.names = names;
	this.util = util;
	this.api = api;
};

WTPageIdSuggestions.prototype.showNames = function() {
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

WTPageIdSuggestions.prototype._display = function() {
	if(this.parentItem == null)
		return;

  this.parentItem.empty();
  if(!this.names.length)
    this.parentItem.css('display', 'none');
  else
    this.parentItem.css('display', 'block');

  if(wtrights["edit-page-metadata"]) {
		if(wtpagenotfound && !wtrights['createpage'])
			return;
    var header = $('<div class="heading"></div>').append($('<b>Page Name Suggestions</b>'));
    this.parentItem.append(header);

  	var wrapper = $('<div style="padding:5px"></div>');
    var list = $('<ul></ul>');
    var me = this;
    for(var i=0; i<this.names.length; i++) {
      var name = this.names[i];
      var item = $('<li>' + name + '&nbsp;&nbsp;</li>');
      var igo = $('<a class="lodbutton"><i class="fa fa-check okbutton"></i> Rename</a>');
      (function(name) {
        igo.click(function(e) { me.movePage(name); });
      })(name);
      item.append(igo);
      list.append(item);
    }
    wrapper.append(list);
    this.parentItem.append(wrapper);
  }
};

WTPageIdSuggestions.prototype.movePage = function( newname ) {
  var curname = mw.config.get('wgPageName');
  var me = this;
  $('html').mask('Renaming ..');
  me.api.movePage( me.title, newname, function(response) {
    if(!response || !response.wtfacts || !response.wtfacts.facts) return;
    if(response.wtfacts.result == 'Success') {
      window.location = newname;
    }
  });
};

WTPageIdSuggestions.prototype.display = function( item ) {
	var me = this;
	this.parentItem = item;
	this._display();
};

WTPageIdSuggestions.prototype.setNames = function( names ) {
  this.names = names;
  this._display();
}
