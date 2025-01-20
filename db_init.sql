CREATE TABLE users (
    username varchar(80),
    email varchar(80),
    password varchar(80),
    token varchar(80)
);

CREATE TABLE todos (
    id SERIAL PRIMARY KEY,
    token varchar(80),
    title varchar(80),
    description varchar(160)
);