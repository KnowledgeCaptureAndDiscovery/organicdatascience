var WTComponent = function(title, details, util, api ) {
	this.title = title;
	this.details = details;
	this.util = util;
	this.api = api;
};

WTComponent.prototype.appendLinkItem = function( list, link, name ) {
	if(!name) name = link;
	var vlink = link.replace(/\s/g, '_');
	list.append($('<li></li>').append($('<a href="'+vlink+'"></a>').append(name)));
};

WTComponent.prototype.populateList = function( list, data ) {
	var me = this;
	//me.appendLinkItem( list, data.location, "<b>DOWNLOAD</b>" );
	console.log(data);

	var idvs = $('<ul></ul>');
	$.each(data.inputdatavars, function(i, v) {
		idvs.append($('<li>'+v+'</li>'));
	});
	list.append($('<li><b>Input</b></li>')).append(idvs);

	if(data.inputparamvars.length) {
		var ipvs = $('<ul></ul>');
		$.each(data.inputparamvars, function(i, v) {
			ipvs.append($('<li>'+v+'</li>'));
		});
		list.append($('<li><b>Param</b></li>')).append(ipvs);
	}

	var odvs = $('<ul></ul>');
	$.each(data.outputdatavars, function(i, v) {
		odvs.append($('<li>'+v+'</li>'));
	});
	list.append($('<li><b>Output</b></li>')).append(odvs);

	if(data.usedintemplates.length) {
		var plinks = $('<ul></ul>');
		$.each(data.usedintemplates, function(i, link) {
			var name = link.replace(/.+\//, '');
			me.appendLinkItem( plinks, link, name );
		});
		list.append($('<li><b>Used in the following Templates:</b></li>')).append(plinks);
	}
};

WTComponent.prototype.getList = function( item, data ) {
	var list = $('<ul></ul>');
	var me = this;

	if(data && data.WTComponent) {
		me.populateList(list, data.WTComponent);
	}
	item.data('list', list);
	return list;
};


WTComponent.prototype.display = function( item ) {
	var me = this;

	item.data('data', me.details);

	var list = me.getList( item, me.details );

	var header = $('<div class="heading"></div>').append($('<b>Component</b>'));
	item.append(header);
	var wrapper = $('<div style="padding:5px"></div>');
	wrapper.append(list);
	item.append(wrapper);
};

