attention:<br>
1.最新的提交已经找到了skew超过cfg设置的20太多的原因，并将skew降低到35以内  <br>
2.在generizetopology.cpp中可以修改cfg.low，我目前定义的标准是将cfg.low设置为cfg.up-20.老师可以自行修改，但是范围越小buffer数量会增加，当然似乎增加的也不是很快，因为这个只影响在原来就属于cfg.low~cfg.up的范围内的buffer <br>
3.最新的提交已经解决了case5在cfglow=cfg.up-2时skew仍然有30的问题。<br>
4.这次我直接在github上修改了generizetopology.cpp,使得在同样情况下，generated的skew相对于之前减小1左右。这次就没有pull下来把main和refgenerated更新了，因此老师要先./sh重新编译main在使用脚本跑，理论上老师跑的generated应该比refgenerated要稍好。

