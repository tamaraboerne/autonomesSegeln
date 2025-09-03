package utils;

public class Tuple<T> {
    public T first;
    public T second;


    public Tuple(T first, T second) {
        this.first = first;
        this.second = second;
    }

    @Override
    public String toString() {
        return "(" + first + ", " + second + ")";
    }
}
