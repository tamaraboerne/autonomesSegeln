package model;

public enum Command {
    Stop(0),
    Start(1),
    Continue(2);

    public final int value;

    Command(int command) {
        value = command;
    }

    public int getValue() {
        return value;
    }

    @Override
    public String toString() {
        return name();
    }
}
