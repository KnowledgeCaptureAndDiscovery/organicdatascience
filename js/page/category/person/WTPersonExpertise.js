var WTPersonExpertise = function(title, expapi, api, details) {
	this.title = title;
	this.expapi = expapi;
	this.api = api;
	this.expertise = details.WTPerson.expertise;
	this.expertiseCount = expapi.personExpertise(title);
};

WTPersonExpertise.prototype.display = function( item ) {
	var me = this;		
	me.$item = item;
	if(!me.expertise)
		return;
	
	me.appendMsg()
	me.appendInput();
	
	$.each(me.expertise, function(k, expertise){
		me.appendExpertise(expertise);
	});
	
	me.closeEdit();
	item.click(function(e){	
		if(me.isOpen)
			me.closeEdit();
		else
			me.openEdit();
	});	
};

WTPersonExpertise.prototype.openEdit = function() {
	var i = this.$item;
	i.find('.edit').show();
	i.find('.nedit').hide();	
	this.isOpen = true;
};
WTPersonExpertise.prototype.closeEdit = function() {
	var i = this.$item;
	i.find('.edit').hide();
	i.find('.nedit').show();
	this.showMsg();
	this.isOpen = false;
};

WTPersonExpertise.prototype.appendMsg = function() {
	var me = this;
	me.$msg = $('<span class="nedit">No Expertise Assigned, click here to assign!</span>');
	me.$item.append(me.$msg);
};

WTPersonExpertise.prototype.appendInput = function() {
	var me = this;
	me.$in = $('<input class="edit" type="text" placeholder="Expertise" style="width:100px;"/>');
	me.$in.autocomplete2({lookup: me.expapi.expertiseSuggestion()});	
	me.$in.keyup(function(e){
		var self = $(this);
		if(e.keyCode == 13){	
			var v = $(this).val();
			if(v != '' && me.expertise.indexOf(v) == -1){
				me.api.addExpertise(me.title, v, null);
				me.addExpertiseLocal(v);
				me.appendExpertise(v);
				self.val('');
				self.autocomplete2().dispose();
				self.autocomplete2({lookup: me.expapi.expertiseSuggestion()});	
				me.openEdit();
				WTTracker.track({
					component: WTTracker.c.persexpert,
					actiontype: WTTracker.t.add,
					action: 'add user expertise',
					taskId: v	
				});
			}				
		}
	});
	me.$in.click(function(e){e.stopPropagation();});
	me.$item.append(me.$in);
};


WTPersonExpertise.prototype.appendExpertise = function( expertise ) {
	var me = this;
	$e = $('<div class="expertise"></div>');
	$txt = $('<span class="label"></span>').append(expertise);
	$count = $('<div class="count nedit"></div>').append(me.count(expertise));
	$del = $('<div class="del edit">&nbsp;</div>');	
	$del.click(function(e){		
		$e = $(this).closest('.expertise');
		var value = $e.find('.label').text();
		me.api.removeExpertise(me.title, value, null);
		me.removeExpertiseLocal(value);
		$e.remove();			
		me.openEdit();
		e.stopPropagation();
		WTTracker.track({
			component: WTTracker.c.persexpert,
			actiontype: WTTracker.t.del,
			action: 'del user expertise',
			taskId: value	
		});
	});
	$e.append($txt).append($count).append($del);
	$e.on( "mouseenter", function(){
		me.highlightTask(expertise);
	});
	$e.on( "mouseleave", function(){
		me.unhighlightTask();
	});
	me.$in.before($e);
};

WTPersonExpertise.prototype.addExpertiseLocal = function(expertise) {
	this.expertise.push(expertise);
};
WTPersonExpertise.prototype.removeExpertiseLocal = function(expertise) {
	if(idx = this.expertise.indexOf(expertise) != -1)
		this.expertise = this.expertise.splice(idx, 1);
};

WTPersonExpertise.prototype.showMsg = function() {
	var me = this;
	if(me.expertise.length==0)
		me.$msg.show();
	else
		me.$msg.hide();	
};

WTPersonExpertise.prototype.count = function(expertise) {
	var me = this;
	var e = expertise.toLowerCase();
	if(e in me.expertiseCount)
		return me.expertiseCount[e];
	return 0;
};

WTPersonExpertise.prototype.highlight = function(expertise) {
	this.$item.find('.expertise .label').each(function(){		
		var e = $(this).text().toLowerCase();
		if(expertise.indexOf(e) == -1)
			$(this).stop(true, true).fadeTo("fast", 0.1, null);
	});
};
WTPersonExpertise.prototype.unhighlight = function() {
	this.$item.find('.expertise .label').each(function(){
		$(this).stop(true, true).fadeTo("fast", 1, null);
	});
};

WTPersonExpertise.prototype.setPersonTask = function(personTask) {
	this.personTask = personTask;
};
WTPersonExpertise.prototype.highlightTask = function(expertise) {
	this.personTask.highlight(expertise);
};
WTPersonExpertise.prototype.unhighlightTask = function() {
	this.personTask.unhighlight();
};
