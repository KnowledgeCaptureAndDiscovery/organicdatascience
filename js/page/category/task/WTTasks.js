var WTTasks = function(title, details, util, api ) {
	this.title = title;
	this.details = details;
	this.util = util;
	this.api = api;
};

WTTasks.prototype.display = function( item ) {
	var me = this;

	var header = $('<div class="heading"></div>').append($('<b>Tasks Answered</b>'));
	item.append(header);
	var wrapper = $('<div style="padding:5px"></div>');
	item.append(wrapper);

	wrapper.append("<div>This page answers the following Tasks:</div>");

	var tasks = me.details.Tasks;
	var list = $('<ul></ul>');
	$.each(tasks, function(name, task) {
		var q_cls = task.exists ? '' : 'new';
		var qlink = $('<a class="'+q_cls+'" href="'+task.key+'"></a>').append(task.text);
		list.append($("<li></li>").append(qlink));
	});

	wrapper.append(list);
	item.append(wrapper);
};

