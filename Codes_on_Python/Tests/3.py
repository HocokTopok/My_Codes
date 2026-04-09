try:
    try:
        print(1/0)
    except ZeroDivisionError as ex:
        new_ex = ArithmeticError("Ty lox")
        raise new_ex from ex
    except Exception as ex:
        print(ex)
    else:
        print("Nice")
    finally:
        print("hahahaha")
except Exception as ex:
    print(ex.__cause__, ex)