<?php

class WTDataset extends WTBase {
	function __construct($title) {
		parent::__construct($title);
	}

	function modifyWikiPage($out) {
		parent::modifyWikiPage($out);
		global $wgScriptPath;
		$old = $out->getHTML();
		$out->clearHTML();

		# -- SCRIPT FOR JSON LD
		list ($error, $facts, $text) = WTFactsAPI::getFacts($this->title->getFullText());
				
		$url1 = "http://".$_SERVER['HTTP_HOST'].$_SERVER['REQUEST_URI'];
		$json_ld_script = '{"@context":"http://schema.org","@type":"Dataset","url":"'.$url1.'"';

		$prop_array=array(
		    'Name (L)' => 'Name',  
		    'Author (L)' => 'Author', 
		    'Contributor (L)' => 'contributor',
		    'CollectedFrom (L)' =>'ContentLocation',
		    'FundedBy (L)' => 'funder',
		    'PartOfCompilation (L)' => 'includedInDataCatalog',
		    'PublishedIn (L)' => 'citation',
		    'DatasetDate (L)'=> 'dateCreated',
		    'DatasetLicense (L)' =>  'license',
		    'DatasetVersion (L)' => 'version',
		    ); 

		 foreach($prop_array as $key=>$value){
		 	$flag = 0;
		 	if($key == 'Author (L)' or $key == 'Contributor (L)')
		 	{
				$flag = 1;
		 	}
		 	
			 if(array_key_exists($key,$facts)==1)
			 {
			 		$prop_value = [];
			 		$prop_str = '';
			 		for ($i=0; $i < count($facts[$key]['values']); $i++)
			 		{
			 			if($flag == 1){
			 				$prop_str = '{"@type":"Person","name":"'.$facts[$key]['values'][$i]['val'].'"}';	
			 			}
			 			else{
			 				$prop_str = '"'.$facts[$key]['values'][$i]['val'].'"';
			 			}			 			
			 			array_push($prop_value,$prop_str);			 			
			 		}
			 		$prop_str = implode(',',$prop_value);
			 		$json_ld_script = $json_ld_script.',"'.$prop_array[$key].'":['.$prop_str.']';
		 	}
		 			
		 }
		$out->addHTML('<script type="application/ld+json">');
		$out->addHTML($json_ld_script."}\n");
		$out->addHTML("</script>\n");

		
		# -- Add the Download LiPD button
		$out->addHTML('<div style="margin-bottom:5px"><a class="lodbutton" href="' .
			$wgScriptPath . '/index.php/Special:WTLiPD?op=export&lipdid=' .
			$this->title.'">Download LiPD</a></div>');

		# -- Add Existing Markup Here
		$out->addHTML( $old );
	}
}

