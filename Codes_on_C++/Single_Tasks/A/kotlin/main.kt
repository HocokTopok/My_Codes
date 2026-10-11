import java.util.ArrayDeque

import java.io.BufferedReader
import java.io.InputStreamReader
import java.io.PrintWriter

data class Point(val x: Int, val y: Int)
data class PointDTO(val x: Int, val y: Int, val distance: Int)

fun main() {
    val reader = BufferedReader(InputStreamReader(System.`in`))
    val writer = PrintWriter(System.out)
    val output = StringBuilder()

    val (n, m) = reader.readLine().split(" ").map { it.toInt() }

    // -1 - стена, 0 - не отмечено пока, >= 1 - текущее растояние, -2 - выход, -3 вход
    val matrixAccess = Array(n) { Array(m) { 0 } }

    var start: Point? = null
    repeat(n) { vertical ->
        reader.readLine().forEachIndexed { ind, ch ->
            when (ch) {
                '#' -> matrixAccess[vertical][ind] = -1
                'S' -> {
                    start = Point(vertical, ind); matrixAccess[vertical][ind] = -3
                }

                'F' -> matrixAccess[vertical][ind] = -2

            }
        }
    }
    reader.close()
    val endPoint = getEndPointDTO(matrixAccess, start ?: return)

    endPoint?.let {
        output.append("${it.distance}\n${getWay(matrixAccess, it)}\n")
    } ?: output.append("-1\n")

    writer.print(output)
    writer.flush()
    writer.close()
}

fun getWay(matrixAccess: Array<Array<Int>>, endPoint: PointDTO): String {
    val way = StringBuilder()

    var x = endPoint.x
    var y = endPoint.y
    var distance = endPoint.distance

    while (distance > 0) {
        if (matrixAccess[x - 1][y] == distance - 1) {
            x--
            way.append("D")
        } else if (matrixAccess[x + 1][y] == distance - 1) {
            x++
            way.append("U")
        } else if (matrixAccess[x][y + 1] == distance - 1) {
            y++
            way.append("L")
        } else if (matrixAccess[x][y - 1] == distance - 1) {
            y--
            way.append("R")
        }
        distance--
    }

    return way.toString().reversed()
}

fun getEndPointDTO(matrixAccess: Array<Array<Int>>, start: Point): PointDTO? {
    val stack = ArrayDeque<PointDTO>()
    stack.addLast(PointDTO(start.x, start.y, 0))

    var endPoint: PointDTO? = null
    while (!stack.isEmpty()) {
        val (x, y, distance) = stack.removeFirst()
        // Проверяем соседей
        if (matrixAccess[x][y] == -2) {
            endPoint = PointDTO(x, y, distance); break
        } else if (matrixAccess[x - 1][y] == -2) {
            endPoint = PointDTO(x - 1, y, distance + 1); break
        } else if (matrixAccess[x + 1][y] == -2) {
            endPoint = PointDTO(x + 1, y, distance + 1); break
        } else if (matrixAccess[x][y - 1] == -2) {
            endPoint = PointDTO(x, y - 1, distance + 1); break
        } else if (matrixAccess[x][y + 1] == -2) {
            endPoint = PointDTO(x, y + 1, distance + 1); break
        }

        // Если ещё не нашли
        if (matrixAccess[x][y - 1] == 0) { // Левый
            stack.addLast(PointDTO(x, y - 1, distance + 1))
            matrixAccess[x][y - 1] = distance + 1
        }
        if (matrixAccess[x - 1][y] == 0) { // Верхний
            stack.addLast(PointDTO(x - 1, y, distance + 1))
            matrixAccess[x - 1][y] = distance + 1
        }
        if (matrixAccess[x + 1][y] == 0) { // Нижний
            stack.addLast(PointDTO(x + 1, y, distance + 1))
            matrixAccess[x + 1][y] = distance + 1
        }
        if (matrixAccess[x][y + 1] == 0) { // Правый
            stack.addLast(PointDTO(x, y + 1, distance + 1))
            matrixAccess[x][y + 1] = distance + 1
        }
    }
    matrixAccess[start.x][start.y] = 0
    return endPoint
}