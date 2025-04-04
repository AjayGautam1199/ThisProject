SamacSys ECAD Model
5360251/1083500/2.49/16/4/Integrated Circuit

DESIGNSPARK_INTERMEDIATE_ASCII

(asciiHeader
	(fileUnits MM)
)
(library Library_1
	(padStyleDef "r155_65"
		(holeDiam 0)
		(padShape (layerNumRef 1) (padShapeType Rect)  (shapeWidth 0.65) (shapeHeight 1.55))
		(padShape (layerNumRef 16) (padShapeType Ellipse)  (shapeWidth 0) (shapeHeight 0))
	)
	(textStyleDef "Default"
		(font
			(fontType Stroke)
			(fontFace "Helvetica")
			(fontHeight 50 mils)
			(strokeWidth 5 mils)
		)
	)
	(patternDef "SOIC127P604X185-16N" (originalName "SOIC127P604X185-16N")
		(multiLayer
			(pad (padNum 1) (padStyleRef r155_65) (pt -2.725, 4.445) (rotation 90))
			(pad (padNum 2) (padStyleRef r155_65) (pt -2.725, 3.175) (rotation 90))
			(pad (padNum 3) (padStyleRef r155_65) (pt -2.725, 1.905) (rotation 90))
			(pad (padNum 4) (padStyleRef r155_65) (pt -2.725, 0.635) (rotation 90))
			(pad (padNum 5) (padStyleRef r155_65) (pt -2.725, -0.635) (rotation 90))
			(pad (padNum 6) (padStyleRef r155_65) (pt -2.725, -1.905) (rotation 90))
			(pad (padNum 7) (padStyleRef r155_65) (pt -2.725, -3.175) (rotation 90))
			(pad (padNum 8) (padStyleRef r155_65) (pt -2.725, -4.445) (rotation 90))
			(pad (padNum 9) (padStyleRef r155_65) (pt 2.725, -4.445) (rotation 90))
			(pad (padNum 10) (padStyleRef r155_65) (pt 2.725, -3.175) (rotation 90))
			(pad (padNum 11) (padStyleRef r155_65) (pt 2.725, -1.905) (rotation 90))
			(pad (padNum 12) (padStyleRef r155_65) (pt 2.725, -0.635) (rotation 90))
			(pad (padNum 13) (padStyleRef r155_65) (pt 2.725, 0.635) (rotation 90))
			(pad (padNum 14) (padStyleRef r155_65) (pt 2.725, 1.905) (rotation 90))
			(pad (padNum 15) (padStyleRef r155_65) (pt 2.725, 3.175) (rotation 90))
			(pad (padNum 16) (padStyleRef r155_65) (pt 2.725, 4.445) (rotation 90))
		)
		(layerContents (layerNumRef 18)
			(attr "RefDes" "RefDes" (pt 0, 0) (textStyleRef "Default") (isVisible True))
		)
		(layerContents (layerNumRef 30)
			(line (pt -3.75 5.25) (pt 3.75 5.25) (width 0.05))
		)
		(layerContents (layerNumRef 30)
			(line (pt 3.75 5.25) (pt 3.75 -5.25) (width 0.05))
		)
		(layerContents (layerNumRef 30)
			(line (pt 3.75 -5.25) (pt -3.75 -5.25) (width 0.05))
		)
		(layerContents (layerNumRef 30)
			(line (pt -3.75 -5.25) (pt -3.75 5.25) (width 0.05))
		)
		(layerContents (layerNumRef 28)
			(line (pt -1.95 4.95) (pt 1.95 4.95) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt 1.95 4.95) (pt 1.95 -4.95) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt 1.95 -4.95) (pt -1.95 -4.95) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt -1.95 -4.95) (pt -1.95 4.95) (width 0.1))
		)
		(layerContents (layerNumRef 28)
			(line (pt -1.95 3.68) (pt -0.68 4.95) (width 0.1))
		)
		(layerContents (layerNumRef 18)
			(line (pt -1.6 4.95) (pt 1.6 4.95) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt 1.6 4.95) (pt 1.6 -4.95) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt 1.6 -4.95) (pt -1.6 -4.95) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -1.6 -4.95) (pt -1.6 4.95) (width 0.2))
		)
		(layerContents (layerNumRef 18)
			(line (pt -3.5 5.12) (pt -1.95 5.12) (width 0.2))
		)
	)
	(symbolDef "SL2_1A" (originalName "SL2_1A")

		(pin (pinNum 1) (pt 0 mils 0 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 230 mils -25 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 2) (pt 0 mils -100 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 230 mils -125 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 3) (pt 0 mils -200 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 230 mils -225 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 4) (pt 0 mils -300 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 230 mils -325 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 5) (pt 0 mils -400 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 230 mils -425 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 6) (pt 0 mils -500 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 230 mils -525 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 7) (pt 0 mils -600 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 230 mils -625 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 8) (pt 0 mils -700 mils) (rotation 0) (pinLength 200 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 230 mils -725 mils) (rotation 0]) (justify "Left") (textStyleRef "Default"))
		))
		(pin (pinNum 9) (pt 1100 mils 0 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 870 mils -25 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 10) (pt 1100 mils -100 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 870 mils -125 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 11) (pt 1100 mils -200 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 870 mils -225 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 12) (pt 1100 mils -300 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 870 mils -325 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 13) (pt 1100 mils -400 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 870 mils -425 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 14) (pt 1100 mils -500 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 870 mils -525 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 15) (pt 1100 mils -600 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 870 mils -625 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(pin (pinNum 16) (pt 1100 mils -700 mils) (rotation 180) (pinLength 200 mils) (pinDisplay (dispPinName false)) (pinName (text (pt 870 mils -725 mils) (rotation 0]) (justify "Right") (textStyleRef "Default"))
		))
		(line (pt 200 mils 100 mils) (pt 900 mils 100 mils) (width 6 mils))
		(line (pt 900 mils 100 mils) (pt 900 mils -800 mils) (width 6 mils))
		(line (pt 900 mils -800 mils) (pt 200 mils -800 mils) (width 6 mils))
		(line (pt 200 mils -800 mils) (pt 200 mils 100 mils) (width 6 mils))
		(attr "RefDes" "RefDes" (pt 950 mils 300 mils) (justify Left) (isVisible True) (textStyleRef "Default"))

	)
	(compDef "SL2.1A" (originalName "SL2.1A") (compHeader (numPins 16) (numParts 1) (refDesPrefix IC)
		)
		(compPin "1" (pinName "DM4") (partNum 1) (symPinNum 1) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "2" (pinName "DP4") (partNum 1) (symPinNum 2) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "3" (pinName "DM3") (partNum 1) (symPinNum 3) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "4" (pinName "DP3") (partNum 1) (symPinNum 4) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "5" (pinName "DM2") (partNum 1) (symPinNum 5) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "6" (pinName "DP2") (partNum 1) (symPinNum 6) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "7" (pinName "DM1") (partNum 1) (symPinNum 7) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "8" (pinName "DP1") (partNum 1) (symPinNum 8) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "16" (pinName "XIN") (partNum 1) (symPinNum 9) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "15" (pinName "XOUT") (partNum 1) (symPinNum 10) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "14" (pinName "VDD18") (partNum 1) (symPinNum 11) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "13" (pinName "VDD33") (partNum 1) (symPinNum 12) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "12" (pinName "GND") (partNum 1) (symPinNum 13) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "11" (pinName "VDD5") (partNum 1) (symPinNum 14) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "10" (pinName "DP") (partNum 1) (symPinNum 15) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(compPin "9" (pinName "DM") (partNum 1) (symPinNum 16) (gateEq 0) (pinEq 0) (pinType Bidirectional))
		(attachedSymbol (partNum 1) (altType Normal) (symbolName "SL2_1A"))
		(attachedPattern (patternNum 1) (patternName "SOIC127P604X185-16N")
			(numPads 16)
			(padPinMap
				(padNum 1) (compPinRef "1")
				(padNum 2) (compPinRef "2")
				(padNum 3) (compPinRef "3")
				(padNum 4) (compPinRef "4")
				(padNum 5) (compPinRef "5")
				(padNum 6) (compPinRef "6")
				(padNum 7) (compPinRef "7")
				(padNum 8) (compPinRef "8")
				(padNum 9) (compPinRef "9")
				(padNum 10) (compPinRef "10")
				(padNum 11) (compPinRef "11")
				(padNum 12) (compPinRef "12")
				(padNum 13) (compPinRef "13")
				(padNum 14) (compPinRef "14")
				(padNum 15) (compPinRef "15")
				(padNum 16) (compPinRef "16")
			)
		)
		(attr "Manufacturer_Name" "CoreChips ShenZhen CO.,Ltd")
		(attr "Manufacturer_Part_Number" "SL2.1A")
		(attr "Mouser Part Number" "")
		(attr "Mouser Price/Stock" "")
		(attr "Arrow Part Number" "")
		(attr "Arrow Price/Stock" "")
		(attr "Description" "USB 2.0 HIGH SPEED 4-PORT HUB CONTROLLER")
		(attr "Datasheet Link" "https://datasheet.lcsc.com/szlcsc/1811151645_CoreChips-SL2-1A_C192893.pdf")
		(attr "Height" "1.85 mm")
	)

)
