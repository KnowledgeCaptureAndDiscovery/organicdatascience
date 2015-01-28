var WTPerson = function(title, details, util, api ) {
	this.title = title;
	this.details = details;
	this.util = util;
	this.api = api;
};

WTPerson.prototype.display = function( item ) {
	var me = this;
	item.data('data', me.details);
	var header = $('<div class="heading"></div>').append($('<b>Recent Contributions</b>'));
	item.append(header);

	var contribs = me.details.WTPerson.contributions;
	if(!contribs.length) {
		var wrapper = $('<div style="padding:5px"></div>');
		wrapper.append('No contributions by this person to the wiki.');
		wrapper.append('<br/><i>Note: If this person exists on the wiki, then add the "Has User ID" property to this page with its value set to the Wiki User ID.</i>'); 
		item.append(wrapper);
		return;
	}

	var table = $('<div class="table contrib"></div>');
	var th = $('<div class="row head"></div>');
	th.append('<div class="cell time">Time</div>');
	th.append('<div class="cell page">Page</div>');
	th.append('<div class="cell comment">Comment</div>');
	table.append(th);
	
	for(var i=0; i<contribs.length; i++) {
		var contrib = contribs[i];

		var timestr = contrib[2];
		var year = timestr.substring(0,4);
		var month = timestr.substring(4,6);
		var day = timestr.substring(6,8);
		var hour = timestr.substring(8,10);
		var minute = timestr.substring(10,12);
		var time = year+"-"+month+"-"+day+" "+hour+":"+minute;

		var tr = $('<div class="row"></div>');
		tr.append('<div class="cell time">'+time+'</div>');
		tr.append('<div class="cell page"><a href="./'+contrib[0]+'">'+contrib[0]+'</a></div>');
		var comment = contrib[1];
		if(comment.length > 100) comment = comment.substring(0, 100) + '...';
		if(comment == '') comment = '-- None --';
		tr.append('<div class="cell comment">'+comment+'</div>');
		table.append(tr);
	}
	item.append(table);
};

