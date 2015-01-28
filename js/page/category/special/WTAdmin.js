var WTAdmin = function(api, baseurl, expapi) {
	this.api = api;
	this.baseurl = baseurl;
	this.expapi = expapi;
};

WTAdmin.prototype.display = function( $item, $admin) {
	var me = this;
	me.modifyHeadline($item);
	if(me.expapi.isLoggedIn() && me.expapi.userid() == "Admin"){
		me.updateDocu($admin);
		me.createTutorial($admin);
	}else{
		me.noPermission($admin);
	}
};

WTAdmin.prototype.modifyHeadline = function( $item ) {
	$icon = $('<div>Admin</div>');
	$icon.css('display', 'inline-block');
	$icon.css('width', '70px');
	$icon.css('margin-right', '5px');
	$icon.css('padding', '0px 3px');
	$icon.css('color', 'white');
	$icon.css('font-size', '20px');
	$icon.css('background', 'black');
	
	$item.prepend($icon);
	$item.css('border-color', 'black');
	$item.css('margin-bottom', '0px');
};

WTAdmin.prototype.noPermission = function( $admin ) {
	$admin.append('<span>You have no permission to access this page!</span><br/>');
};

WTAdmin.prototype.updateDocu = function( $admin ) {
	var me = this;
	$loading = $('<img src="'+me.baseurl+'/extensions/WorkflowTasks/images/loading.gif"/>');
	$loading.hide();	
	$btn = $('<button><span>Update Documenation </span></button>');
	$btn.append($loading);
	$btn.click(function(){
		$btn.attr("disabled","disabled");
		$loading.show();
		me.api.updateDocu(function(){
			$loading.hide();
			$btn.removeAttr("disabled");
		});
	});
	$admin.append('<h2>Documenation</h2>');
	$admin.append($btn);
	$admin.append('<br/>');
	$admin.append('<br/>');
}

WTAdmin.prototype.createTutorial = function( $admin ) {
	var me = this;
	$fullname = $('<input placeholder="Firstname Lastname" />');
	$username = $('<input placeholder="Username" />');
	$submit = $('<button>Create Tutorial</button>');
	$cancel = $('<button>Cancel</button>');
	
	$tutorial = $('<div></div>');
	$tutorial.append($fullname).append('<br\>');
	$tutorial.append($username).append('<br\>');
	$tutorial.append($submit);
	$tutorial.append($cancel);
	
	$admin.append('<h2>Add User Training</h2>');
	$admin.append($tutorial);
	$admin.append('<br/>');
	$admin.append('<br/>');
	
	$cancel.click(function(){
		$username.val('');
		$fullname.val('');
	});
	$submit.click(function(){
		var u = $username.val();
		var f = $fullname.val();
		if(u != '' && f != ''){
			$tutorial.mask(lpMsg('Adding User Training... this may take several minutes...'));
			me.api.addTraining(f, u, function(){
				$username.val('');
				$fullname.val('');
				$tutorial.unmask();
			});
		}
	});	
}