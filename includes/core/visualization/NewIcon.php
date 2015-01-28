<?php
require_once 'Icon.php';

class NewIcon extends Icon{
	
	private $radius = 10;
	private $width = 14;
	private $height = 5;
	
	protected function renderSvg(){	
		$x = $this->radius-$this->width/2+2;
		$y = $this->radius-$this->height/2+2;
		$cx = $this->radius+2;
		$cy = $this->radius+2;

		$svg = '<rect x="'.$x.'" y="'.$y.'" width="'.$this->width.'" height="'.$this->height.'" style="fill:#d9d9d9;" />';
		$svg .= '<rect x="'.$y.'" y="'.$x.'" width="'.$this->height.'" height="'.$this->width.'" style="fill:#d9d9d9;" />';
		return $svg;
	}

}

