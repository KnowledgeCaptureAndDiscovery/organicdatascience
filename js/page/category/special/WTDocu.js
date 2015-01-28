var WTDocu = function(baseurl) {
	this.baseurl = baseurl;
};

WTDocu.prototype.display = function( item, docu) {
	var me = this;
	$icon = $('<div>Docu</div>');
	$icon.css('display', 'inline-block');
	$icon.css('width', '55px');
	$icon.css('margin-right', '5px');
	$icon.css('padding', '0px 3px');
	$icon.css('color', 'white');
	$icon.css('font-size', '20px');
	$icon.css('background', '#a7d7f9');
	
	item.prepend($icon);
	item.css('border-color', '#a7d7f9');
	item.css('margin-bottom', '0px');
	
	$('#contentSub').remove();
	$docu = $('#wt-documentation');
	$docu.find('.menu .item').click(function(){
		var title = $(this).text();
		title = $.trim(title);
		title = title.replace(' ', '_');
		document.location.href = me.baseurl+'/index.php/'+title;
	});
	$docu.find('#toc').remove();
	WTTracker.trackHover($docu.find('.menu'), WTTracker.c.docu);
};
