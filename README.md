最新的提交已经找到了skew超过cfg设置的20太多的原因，并将skew降低到35以内  <br>
在generizetopology.cpp中可以修改cfg.low，我目前定义的标准是将cfg.low设置为cfg.up-20.老师可以自行修改，但是范围越小buffer数量会增加，当然似乎增加的也不是很快，因为这个只影响在原来就属于cfg.low~cfg.up的范围内的buffer
