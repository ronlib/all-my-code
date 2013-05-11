import rpyc

class MyService(rpyc.Service):
    
    ALIASES = ["My Simple Service"]        
        
    def on_connect(self):
       
        
        # code that runs when a connection is created
        # (to init the serivce, if needed)
        pass

    def on_disconnect(self):
        # code that runs when the connection has already closed
        # (to finalize the service, if needed)
        pass

    def exposed_inner_service_conn(self):
        
        self.exposed_c2 = None
        try:
            self.exposed_c2 = rpyc.connect("localhost", 18861)
            return self.exposed_c2
        except Exception, e:
            print e.message
            self.exposed_c2.close()
        
        return None


def run():
    from rpyc.utils.server import ThreadedServer
    t = ThreadedServer(MyService, port = 18863)   
    t.start()   

if __name__ == "__main__":   
    run()
