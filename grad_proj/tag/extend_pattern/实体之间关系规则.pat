<MED>@\d+主要用于<DIS>@\d+ MED-DIS|+ 0-1
<MED>@\d+可用于<DIS>@\d+ MED-DIS|+ 0-1
<MED>@\d+可用于.{1,6}的<DIS>@\d+ MED-DIS|+ 0-1
<DIS>@\d+和<DIS>@\d+时禁用<MED>@\d+ MED-DIS|- 2-0 2-1
<DIS>@\d+和<DIS>@\d+患者禁用<MED>@\d+ MED-DIS|- 2-0 2-1
<MED>@\d+和<MED>@\d+可减少<DIS>@\d+的危险 MED-DIS|+ 0-2 1-2
<MED>@\d+对<DIS>@\d+病人无益 MED-DIS|- 0-1
<MED>@\d+和<MED>@\d+可增加<DIS>@\d+发作 MED-DIS|+ 0-2 1-2
有<DIS>@\d+高危的患者仍需<MED>@\d+抗凝治疗 MED-DIS|+ 1-0
<MED>@\d+是用来控制<DIS>@\d+时.{0,4}的药物 MED-DIS|+ 0-1
<MED>@\d+也是<DIS>@\d+时控制.{0,4}的.{0,4}药物 MED-DIS|+ 0-1
<MED>@\d+仅在<DIS>@\d+患者作为一线治疗 MED-DIS|+ 0-1
对<DIS>@\d+患者用<MED>@\d+抗凝治疗作为脑血管栓塞的一级预防 MED-DIS|+ 1-0
有<DIS>@\d+的患者，<MED>@\d+要慎用 MED-DIS|- 1-0
<MED>@\d+\(.{1,11}\)对<DIS>@\d+患者<DIS>@\d+的一级预防也有效 MED-DIS|+ 0-2 0-3
具有<DIS>@\d+高危的患者都需要以<MED>@\d+作长期抗凝治疗 MED-DIS|+ 1-0
用<MED>@\d+.{0,4}治疗显著降低<DIS>@\d+的发生率 MED-DIS|+ 0-1
用<MED>@\d+(?:可以|都可)(?:减少|降低)<DIS>@\d+的发生率 MED-DIS|+ 0-1
推荐<MED>@\d+可作为<DIS>@\d+.{0,4}的药物 MED-DIS|+ 0-1
<DIS>@\d+患者，静脉应用或口服<MED>@\d+是有效和安全的 MED-DIS|+ 1-0
用<MED>@\d+有助于防止<DIS>@\d+发作 MED-DIS|+ 0-1
若<DIS>@\d+患者有<SYM>@\d+，避免使用<MED>@\d+ MED-DIS|- 2-0
采用<MED>@\d+预防<DIS>@\d+复发 MED-DIS|+ 0-1
<MED>@\d+是用于终止.{0,4}/<DIS>@\d+的主要药物 MED-DIS|+ 0-1
<MED>@\d+：为目前广为推荐静脉应用终止<DIS>@\d+的药物 MED-DIS|+ 0-1
<DIS>@\d+早期就应接受<MED>@\d+治疗 MED-DIS|+ 1-0
<MED>@\d+提示可降低<DIS>@\d+死亡 MED-DIS|+ 0-1
发生明显<DIS>@\d+，可静脉(?:输注|滴注)<MED>@\d+ MED-DIS|+ 1-0
给予<MED>@\d+防止<DIS>@\d+ MED-DIS|+ 0-1
<DIS>@\d+者给予<MED>@\d+ MED-DIS|+ 1-0
发生<DIS>@\d+或<DIS>@\d+，推荐(?:予|给予)<MED>@\d+、<MED>@\d+及<MED>@\d+等 MED-DIS|+ 2-0 2-1 3-0 3-1 4-0 4-1
给予<MED>@\d+(?:\d+d|\d+D|)防治<DIS>@\d+ MED-DIS|+ 0-1
有<DIS>@\d+者应该(?:长期|坚持|)服用<MED>@\d+ MED-DIS|+ 1-0
<DIS>@\d+住院患者做<TRE>@\d+ TRE-DIS 1-0
<SYM>@\d+很容易由<TRE>@\d+作出诊断 TRE-SYM 1-0
<SYM>@\d+也可以见于<DIS>@\d+ DIS-SYM 1-0
<TRE>@\d+或<TRE>@\d+来确定有无<DIS>@\d+ TRE-DIS 0-2 1-2
<SYM>@\d+是<DIS>@\d+的最早的表现 DIS-SYM 1-0
<SYM>@\d+可引起<DIS>@\d+ DIS-SYM 1-0
<TRE>@\d+能够(?:成功地治疗|治愈)<DIS>@\d+ TRE-DIS 0-1
采用<TRE>@\d+\([\w\s,，]+\)治疗<DIS>@\d+ TRE-DIS 0-1
引起<SYM>@\d+或<SYM>@\d+，导致<DIS>@\d+ DIS-SYM 2-0 2-1
严重的<DIS>@\d+往往需要外科手术，甚至<TRE>@\d+ TRE-DIS 1-0
发现有<DIS>@\d+、<DIS>@\d+、<DIS>@\d+、<DIS>@\d+等.{0,4}征象时，可给予全身<TRE>@\d+或<TRE>@\d+ TRE-DIS 4-0 4-1 4-2 4-3 5-0 5-1 5-2 5-3
<TRE>@\d+监测(?:无|有|有无)<DIS>@\d+ TRE-DIS 0-1
