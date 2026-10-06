
# 											github

### github网站访问

GitHub是世界范围内比较有影响力的，项目库托管网站，大量的私人工程或企业及项目在此网站托管发布，里面也包含大量开源的内容，是软件研发工程的工具类网站。

#### github网站查询项目通常有以下几种方式

- Topic 分类查看
- trending 推送查看
- 搜索栏按关键字查询内容
- 按标签查询（sample，tutorial）    xxxx  sample

### <font color="red">项目结构</font>

- <font color= "#FF8C00">仓库：工程存储单位，一般一个仓库中存储一个独立项目，一个用户可以有若干个仓库</font>

- <font color="#1E90FF">CODE：存储开源数据，开源代码，后续用户下载开源项目，下载就是code中的全部内容</font>
- <font color="#1E90FF">ISSUES，问答板块，解决项目异常，提交bug</font>
- <font color="#1E90FF">README.MD 工程自述文件，进行项目介绍，版本答疑，使用markdown语言编写</font>
- <font color="#1E90FF">许可证：GPL3.0 , Apphache 2.0 , MIT ,这些许可证给使用者最小的限制，最大的权力</font>

### git的配置，工程的上传与下载

- <font color="red">云端仓库（托管到github中）</font>
  - 设备认证，生成密钥串，粘贴到github仓库中，让设备受信任 后续可以完成内容的上传
  - 创建本地仓库，出现（master）标志，表示在仓库所在位置，默认.git仓库是隐藏文件，使用<font color="#1E90FF">git init</font>

- <font color = "#FF8C00">关于（master）分支概念：</font>
  - 分支：资源存储单位，仓库包含分支，默认情况下仓库都有主分支，默认所有数据都向主分支存储，一个仓库可以有多个分支
  - 多人协作开发，对分支进行管理合并，一系列相关命令，创建分支，删除分支，分支选择等等

![alt](https://img.remit.ee/i/By8Q3hI9isYz) 



<font color = "red">后续上传时，分支名相同则合并，不同则在云端创建新分支，存储用户上传内容</font>

### git上传的几条命令

- <font color = "blue">ssh -T git@github.com</font>      #测试设备是否关联成功
- <font color = "blue">git config --list </font>                    #查看git本地配置文件
- <font color = "blue">git config --global user.email "your email"</font>
- <font color = "blue">git config --global user.name "your name"</font>

### 生成密钥文件，传输加密方式选择非对称rsa加密（设备指纹）

<font color = "blue">ssh-keygen -t rsa -C "your email"</font>  <font color ="red">*记住密钥生成的位置，找到密钥文件，复制密钥串</font>

根据提供的位置，打开.pub密钥文件，复制其中密钥字符串，粘贴到指定位置

步骤：

- 头像（menu）--> Settings --> SSH and GPG key  --> New SSH key --> 粘贴密钥 --> add SSH key
- 再次使用ssh -T git@github 测试关联

### 本地数据上传过程，以及版本更新

本地数据与云端数据为相互的依赖关系，本地为新版，云端为旧版（发行版）版本更新使用本地新内容同步给云端

了解本地数据到云端同步（增，删，改）



#### 掌握git命令，git全程分布式版本控制系统，可以让开发者在本地电脑通过命令远程访问修改仓库的数据和内容

<font color = "red">*上传可以以目录为单位，也可以是单个文件</font>

<font color = "red">使用 git remote命令创建ssh地址别名</font>

![alt](https://img.remit.ee/i/eceUjfHzPufr)

### <font color = "orange">commit提交</font>

- 用户的每次提交，commit系统进行代码的备份，进行交叉对比，有一个提交列表存储这些备份，可以通过提交功能回溯到任意时刻删除或修改的位置

###  下载开源项目

- 所有以.git仓库为单位的操作都与开发有关，只是打包下载开源代码和资源文件而已
- 命令下载：git clone "工程https地址"
