
var WTDashboard = function() {
	console.log(JSON.stringify(dashboard));
	var $dashboard = $('#dashboard');	
	$dashboard.append('<canvas id="collaboration-graph" width="800" height="600"></canvas>');
	var sys = arbor.ParticleSystem(1000, 400,1);
	sys.parameters({gravity:true});
	sys.renderer = Renderer("#collaboration-graph") ;
	
	var nodes = {};
	$.each(dashboard.collaborationgraph.nodes, function(k, n){
		nodes[n.id] = {'color':'#ccc','shape':'rectangle','label': n.label};
	});
	
	var edges = {};
	$.each(dashboard.collaborationgraph.edges, function(k, e){
		var evalue = {'weight':e.size, 'color':'green'};
		if(edges.hasOwnProperty(e.source)){
			edges[e.source][e.target] = evalue;
		}else{
			var nodeedge = {}
			nodeedge[e.target] = evalue;
			edges[e.source] = nodeedge;
		}			
	});

	var graphdata = {nodes:nodes, edges: edges};
	console.log(JSON.stringify(graphdata));
	
	
//	var data = {
//		nodes:{
//			animals:{'color':'#ccc','shape':'rectangle','label':'Animalsdddd'},
//			dog:{'color':'#ccc','shape':'rectangle','label':'dog'},
//			cat:{'color':'#ccc','shape':'rectangle','label':'cat'}
//		},
//		edges:{
//			animals:{ dog:{weight:3}, cat:{} }
//		}
//		};
//	data = {nodes:nodes, edges: edges};
	sys.graft(graphdata);	

};

WTDashboard.prototype.display = function( $item, $admin) {
	var me = this;

	
};


$(function() {
	new WTDashboard();    
});