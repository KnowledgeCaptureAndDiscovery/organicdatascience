var WTCredits = function(title, tree, util, api ) {
	this.title = title;
	this.tree = tree;
	this.util = util;
	this.api = api;
};

WTCredits.prototype.display = function( item ) {
	var me = this;

	var header = $('<div class="heading"></div>').append($('<b>Credits</b>'));
	item.append(header);
	var wrapper = $('<div style="padding:5px"></div>');
	if(wtcategories['Task']) 
		wrapper.append("<div>Users who have contributed to this Task, its SubTasks and Answers:</div>");
	else 
		wrapper.append("<div>Users who have contributed to this Page:</div>");

	var contributors = me.tree.Contributors;
	var list = $('<ul></ul>');
	$.each(contributors, function(name, details) {
		var userlink = $("<a href='./User:"+name+"'>"+name+"</a>");
		list.append($("<li></li>").append(userlink).append(" ("+details[1]+" Edits)"));
	});
	wrapper.append(list);
	item.append(wrapper);
};

