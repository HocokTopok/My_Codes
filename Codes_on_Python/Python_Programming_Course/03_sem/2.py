some_var = 100 # global

def fun_1():
    some_var_1 = 101 # enclosing
    
    def fun_2():
        some_var_2 = 102 # enclosing
        
        def fun_3():
            some_var_3 = 103 # enclosing

            def fun_4():
                # nonlocal some_var_1, some_var_2, some_var_3
                print(some_var_1, some_var_2, some_var_3)
                # some_var = 104 # local
