var WTTaskContext = function(expapi, $headline, $subTasks, $timeline) {
	this.expapi = expapi;
	this.expapi.subscribe(this);
	this.$subTasks = $subTasks;
	this.$timeline = $timeline;	
	this.isTimeline = false;
	this.initContextDiv($headline);
};

WTTaskContext.prototype.display = function() {
	var me = this;	
	me.ctx = me.expapi.context();
	me.initTemplate();
	me.initParent();
	me.initHeadline();
	me.initTabs();
};

WTTaskContext.prototype.notify = function() {
	this.display();
};

WTTaskContext.prototype.initContextDiv = function($headline) {
	this.$ctx = $('<div id="context"></div>');
	$headline.replaceWith(this.$ctx);
	WTTracker.trackHover(this.$ctx, WTTracker.c.context);
};

WTTaskContext.prototype.initTemplate = function() {
	var html = '<div class="table parent">';
		html +='	<div class="row">';
		html +='		<div class="cell icon"></div>';
		html +='		<div class="cell title"><a></a></div>';
		html +='	</div>';		
		html +='</div>';
		html +='<div class="table headline">';
		html +='	<div class="row">';
		html +='		<div class="cell dots"></div>';
		html +='		<div class="cell icon"></div>';
		html +='		<div class="cell title"></div>';
		html +='		<div class="cell tabs"></div>';
		html +='	</div>';
		html +='</div>';	
	this.$ctx.html(html);	
};

WTTaskContext.prototype.initParent = function() {
	var me = this;
	if(me.ctx.parent){
		var $icon = me.$ctx.find('.parent .icon');
		$icon.css('background-image','url('+me.ctx.parent.icon+')');
		var $title = me.$ctx.find('.parent .title a');
		$title.html(me.ctx.parent.title);
		$title.attr('href', me.ctx.parent.id);
		$title.click(function(e){
			e.preventDefault();
	        WTTracker.track({
	            component: WTTracker.c.context,
	            actiontype: WTTracker.t.nav,
	            action: 'open task',
	            taskId: me.ctx.parent.id,
	            callback: function(){
	            	location.href  = me.ctx.parent.id
	            }
	        });
		});
	}else{
		me.$ctx.find('.parent').remove();
		me.$ctx.find('.headline .dots').remove();
	}
};

WTTaskContext.prototype.initHeadline = function() {
	var me = this;
	var $icon = me.$ctx.find('.headline .icon');
	$icon.css('background-image','url('+me.ctx.selected.icon+')');
	var $title = me.$ctx.find('.headline .title');
	$title.html(me.ctx.selected.title);
};

WTTaskContext.prototype.initTabs = function() {
	var me = this;
	var $tabs = me.$ctx.find('.headline .tabs');
	me.$tTab = $('<div class="tab">Timeline</div>');	
	me.$sTab = $('<div class="tab">SubTasks</div>');	
	$tabs.append(me.$tTab)
	$tabs.append(me.$sTab);
	if(me.isTimeline)
		me.openTimeline();
	else
		me.openSubTasks();
	me.$tTab.click(function(){
		me.openTimeline();
	});
	me.$sTab.click(function(){		
		me.openSubTasks();
	});
};

WTTaskContext.prototype.openTimeline = function() {
	var me = this;
	me.$sTab.removeClass('selected');
	me.$tTab.addClass('selected');
	me.$subTasks.hide();
	me.$timeline.show();
	me.isTimeline = true;
};

WTTaskContext.prototype.openSubTasks = function() {
	var me = this;
	me.$tTab.removeClass('selected');
	me.$sTab.addClass('selected');
	me.$timeline.hide();
	me.$subTasks.show();
	me.isTimeline = false;
};