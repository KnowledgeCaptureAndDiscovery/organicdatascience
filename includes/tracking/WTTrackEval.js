var WTTrackEval = function(apiurl, $table) {
	this.apiurl = apiurl;	
	this.$table = $table;
	this.initDelete();
};

WTTrackEval.prototype.init = function() {
	var me = this;
	$.post( me.apiurl, {call: 'read'}, function( data ) {		
		me.createTable(data);
		setTimeout(function(){me.init()}, 3000);
	});
};

WTTrackEval.prototype.initDelete = function() {
	var me = this;
	$('#del').click(function(){
		$.post(me.apiurl,{call:'del'});
	});	
};

WTTrackEval.prototype.createTable = function(data) {
	var me = this;
	me.$table.html('');
	var headline = {
		component: 'Component',
		subcomponent: 'SubComp',
		actiontype: 'ActionType',
		action: 'Action',		
		taskId: 'taskId',
		value: 'Value',
		user: 'User',
		time: 'Time',
		domain: 'Domain'
	};
	me.$table.append(me.createRow(headline, true));
	$.each(data, function( k, track ) {		
		me.$table.append(me.createRow(track, false));		
	});	
};

WTTrackEval.prototype.createRow = function(track, head) {
	var me = this;	
	var $row = $('<div></div>');
	if(head) $row.addClass('head');
	$row.append(me.createCell(track.component));
	$row.append(me.createCell(track.subcomponent));
	$row.append(me.createCell(track.action));
	$row.append(me.createCell(track.actiontype));
	$row.append(me.createCell(track.taskId));
	$row.append(me.createCell(track.value));
	$row.append(me.createCell(track.user));
	$row.append(me.createCell(track.time));
	$row.append(me.createCell(track.domain));
	return $row;
};

WTTrackEval.prototype.createCell = function(item) {
	var $cell = $('<div></div>');
	$cell.html(item);
	return $cell;
};



