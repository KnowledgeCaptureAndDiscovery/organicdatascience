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
		$json_ld_script = $this->getJsonLD();
		$out->addHTML('<script type="application/ld+json">');
		$out->addHTML($json_ld_script);
		$out->addHTML("</script>\n");

		
		# -- Add the Download LiPD button
		$out->addHTML('<div style="margin-bottom:5px"><a class="lodbutton" href="' .
			$wgScriptPath . '/index.php/Special:WTLiPD?op=export&lipdid=' .
			$this->title.'">Download LiPD</a></div>');

		# -- Add Existing Markup Here
		$out->addHTML( $old );
	}

	function getLEPropValue($prop, $facts) {
		$leprop = $prop . " (L)";
		if(array_key_exists($leprop, $facts)) {
			return $facts[$leprop]['values'][0]['val'];
		}
		return null;
	}

	function getJsonLD() {
		list ($error, $facts, $text) = WTFactsAPI::getFacts($this->title->getFullText());
		global $wgServer, $wgScriptPath;
		$url1 = "http://".$_SERVER['HTTP_HOST'].$_SERVER['REQUEST_URI'];
		$jsonld = array("@context"=>"http://schema.org", "@type"=>"Dataset", "url"=>$url1, "@id"=>$url1);

		$prop_array=array(
		    'Name (L)' => 'name',  
		    'Author (L)' => 'author', 
		    'Contributor (L)' => 'contributor',
		    'CollectedFrom (L)' =>'spatialCoverage',
		    'FundedBy (L)' => 'funder',
		    'PartOfCompilation (L)' => 'includedInDataCatalog',
		    'PublishedIn (L)' => 'citation',
		    'DatasetDate (L)'=> 'dateCreated',
		    'Description (L)' =>  'description',
		    'DatasetLicense (L)' =>  'license',
		    'DatasetVersion (L)' => 'version'
		    ); 

		foreach($prop_array as $key=>$pvalue) {
			if(array_key_exists($key,$facts)) {
				$prop_values = [];
				$prop_value = null;
				$values = $facts[$key]['values'];
				foreach($values as $value) {
					$val = $value['val'];
					$prop_value = $val;
					if($key == 'Author (L)' or $key == 'Contributor (L)') {
						$val = preg_replace("/_/", " ", $val);
			 			$prop_value = array("@type"=>"Person","name"=>$val);
					}
					else if($key == 'PublishedIn (L)') {
						list ($cerror, $cfacts, $ctext) = WTFactsAPI::getFacts($val);
						$cname = $this->getLEPropValue("Title", $cfacts);
						$link = $this->getLEPropValue("HasLink", $cfacts);
						$doi = $this->getLEPropValue("HasDOI", $cfacts);
			 			$prop_value = array(
							"@type"=>"CreativeWork",
							"identifier"=>$doi,
							"name"=>$cname,
							"url"=>$link
						);
					}
					else if($key == 'CollectedFrom (L)') {
						list ($cerror, $cfacts, $ctext) = WTFactsAPI::getFacts($val);
						$lat = $this->getLEPropValue("Wgs84:Lat", $cfacts);
						$long = $this->getLEPropValue("Wgs84:Long", $cfacts);
						$alt = $this->getLEPropValue("Wgs84:Alt", $cfacts);
			 			$prop_value = array(
							"@type"=>"Place",
							"geo"=>array(
								"@type"=>"GeoCoordinates",
								"latitude"=>$lat,
								"longitude"=>$long,
								"elevation"=>$alt
							)
						);
					}
					else if($key == 'FundedBy (L)') {
						list ($cerror, $cfacts, $ctext) = WTFactsAPI::getFacts($val);
						$agency = $this->getLEPropValue("FundingAgency", $cfacts);
			 			$prop_value = array(
							"@type"=>"Organization",
							"name"=>$agency
						);
					}
					if($prop_value)
			 			array_push($prop_values, $prop_value);			 			
				}
				$jsonld[$pvalue] = $prop_values;
		 	}
		}
		$jsonld["keywords"] = "paleoclimate, climate";
		if(array_key_exists("Keywords", $facts)) {
			$values = $facts["Keywords"]['values'];
			foreach($values as $value) {
				$val = $value['val'];
				$jsonld["keywords"] = $val;
			}
		}

		$downloadUrl = $wgServer . $wgScriptPath . '/index.php/Special:WTLiPD?op=export&lipdid=' .  $this->title;
		$jsonld['distribution'] = array(
			array(
				"@type"=>"DataDownload",
				"encodingFormat"=>"LPD",
				"contentUrl"=>$downloadUrl
			));
		return json_encode($jsonld);
	}
}

