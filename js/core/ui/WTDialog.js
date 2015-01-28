var WTDialog = function(data) {	
	var me = this;
	me.data = data;
	me.initTemplate();	
	me.initData();
	me.initEvents();
	me.openDialog();	
};

WTDialog.prototype.initTemplate = function() {
	var html  = '<div class="wt-dialog">';
		html += '	<div class="head">';
		html += '		<div class="icon"></div>';
		html += '		<div class="title"></div>';
		html += '		<div class="close"></div>';
	    html += '	</div>';
		html += '	<div class="content"></div>';
		html += '	<div class="options"></div>';
	    html += '</div>';
	this.$html = $(html);
};

WTDialog.prototype.initData = function() {
	var me = this;
	me.$html.find('.icon').css('background-image', 'url('+me.data.icon+')');
	me.$html.find('.title').text(me.data.title);
	me.$html.find('.content').html(me.data.content);
};

WTDialog.prototype.initEvents = function() {
	var me = this;
	$opts = me.$html.find('.options');
	$.each(me.data.options, function(k, opt) {
		var $opt = $('<button></button>').text(opt);
		$opt.click(function(){
			me.$html.trigger('close');
			var call = me.data.callback;
			if(call)
				call.func.apply(call.obj, [opt, call.data]);
		});
		$opts.append($opt);
	});
	me.$html.click(function(e) {
		e.preventDefault();
	});
};

WTDialog.prototype.openDialog = function() {
	this.$html.lightbox_me({destroyOnClose:true, centered:true});
};