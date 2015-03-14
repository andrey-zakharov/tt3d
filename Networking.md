#Simplified. Overall application scheme

**Orange** boxes are RPC,
**Green** - engine and
**Yellow** - ready to use
# High level #
## Server side ##
![http://yuml.me/4ccb280f?.png](http://yuml.me/4ccb280f?.png)
```
[Server{bg:blue}]<>-1[ConnectionTemplate{bg:yellow}]
[ConnectionTemplate]<>-*[RpcService{bg:yellow}]
[RpcService]^[<<RpcService>>;Clients{bg:orange}]
[RpcService]^[<<RpcService>>;Landscape{bg:orange}]
[RpcService]^[<<RpcService>>;Vehicles{bg:orange}]
[RpcService]^[<<RpcService>>;Tracks{bg:orange}]
[RpcService]^[<<RpcService>>;Economy{bg:orange}]
[<<RpcService>>;Clients]<-.->[<<manager>>;Clients{bg:green}]
[<<RpcService>>;Landscape]<-.->[<<manager>>;Landscape{bg:green}]
[<<RpcService>>;Vehicles]<-.->[<<manager>>;Vehicles{bg:green}]
[<<RpcService>>;Tracks]<-.->[<<manager>>;Tracks{bg:green}]
[<<RpcService>>;Economy]<-.->[<<manager>>;Economy{bg:green}]
[<<manager>>;Clients]--buy land>[<<manager>>;Landscape]
[<<manager>>;Clients]<-->[<<manager>>;Vehicles]
[<<manager>>;Tracks]<build--owned by>[<<manager>>;Clients]
[<<manager>>;Landscape]<lay on--weighed down by>[<<manager>>;Tracks]
[<<manager>>;Tracks]<drive on-used for>[<<manager>>;Vehicles]
#[<<manager>>;Landscape]<-->[Physics]
[<<manager>>;Vehicles]<-->[Physics]
[<<manager>>;Tracks]<-->[Physics]
[Physics]^-.->[<<implementation>>;PhysicsEngine]
```
## Client side ##
![http://yuml.me/30269690?.png](http://yuml.me/30269690?.png)
```
# Client. Simplified
# yellow - ready to use
# orange - RPC layer
# green - engine layer
[Renderer]^-.->[<<implementation>>;RendererEngine]
[GUI]^-.->[<<implementation>>;GUIEngine]
[Client{bg:red}]<>-*[ClientConnection{bg:yellow}]
[<<RpcStub>>;Clients{bg:orange}]++-1[ClientConnection]
[<<RpcStub>>;Landscape{bg:orange}]++-1[ClientConnection]
[<<RpcStub>>;Tracks{bg:orange}]++-1[ClientConnection]
[<<RpcStub>>;Vehicles{bg:orange}]++-1[ClientConnection]
[<<RpcStub>>;Economy{bg:orange}]++-1[ClientConnection]
[<<Controller>>;Clients{bg:green}]++-1[<<RpcStub>>;Clients]
[<<Controller>>;Landscape{bg:green}]++-1[<<RpcStub>>;Landscape]
[<<Controller>>;Tracks{bg:green}]++-1[<<RpcStub>>;Tracks]
[<<Controller>>;Vehicles{bg:green}]++-1[<<RpcStub>>;Vehicles]
[<<Controller>>;Economy{bg:green}]++-1[<<RpcStub>>;Economy]
[Renderer]<->[<<Controller>>;Clients]
[Renderer]<->[<<Controller>>;Landscape]
[Renderer]<->[<<Controller>>;Tracks]
[Renderer]<->[<<Controller>>;Vehicles]
[Renderer]<->[<<Controller>>;Economy]
[Renderer]<-[GUI]
[Input]->[GUI]
[GUI]<->[<<Controller>>;Clients]
[GUI]<->[<<Controller>>;Landscape]
[GUI]<->[<<Controller>>;Tracks]
[GUI]<->[<<Controller>>;Vehicles]
[GUI]<->[<<Controller>>;Economy]
```

## Low level design ##
![http://yuml.me/32c92f29?.png](http://yuml.me/32c92f29?.png)

```
[protobuf::RpcController]^[RpcController]
[RpcController]^[Connection]
[protobuf::RpcChannel]^[Connection{bg:white}]
[Connection]^[ProtobufConnection]
[ProtobufConnection]^[ClientConnection]
[<<noncopyable>>]^-.-[RawConnection]
[RawConnection]^[RawConnectionImpl]
[RawConnectionImpl]uses -.->[Decoder]
[RawConnectionImpl]^[RawProtobufConnection]
[RawProtobufConnection]uses -.->[ProtobufDecoder]
[Connection]<>1-1[RawConnection]
[Server{bg:blue]<>-*[Connection]
[Server]-.-[ProtobufConnection]
[Client{bg:red}]<>-1[Connection]
[Client]-.-[ClientConnection]
[Decoder]-.-[ProtobufDecoder]
```