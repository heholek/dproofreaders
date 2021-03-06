<?php

class NonexistentNonactivatedUserException extends Exception { }

class NonuniqueNonactivatedUserException extends Exception { }

class NonactivatedUser
{
    private $table_row;

    // List of fields that can never be set outside of this class
    private $unsettable_fields = array(
        "id",
        "date_created",
    );

    // List of fields that when set should never change
    private $immutable_fields = array(
        "username",
    );

    // Fields are assumed to be strings unless included here
    private $integer_fields = array(
        "email_updates",
    );

    public function __construct($username=NULL)
    {
        if($username !== NULL)
        {
            $this->load("username", $username);
        }
    }

    // The __set() and __get() methods allow access to user fields without
    // creating accessors for them all individually.
    // See the PHP docs for "magic methods".
    public function __set($name, $value)
    {
        if(in_array($name, $this->unsettable_fields))
        {
            throw new DomainException(sprintf(
                _("%s is an unsettable field"), $name)
            );
        }

        if(isset($this->$name) && in_array($name, $this->immutable_fields))
        {
            throw new DomainException(sprintf(
                _("%s is an immutable field"), $name)
            );
        }

        $this->table_row[$name] = $value;
    }

    public function __get($name)
    {
        return $this->table_row[$name];
    }

    public function __isset($name)
    {
        return isset($this->table_row[$name]);
    }

    private function load($field, $value, $strict=TRUE)
    {
        if(!in_array($field, $this->integer_fields))
        {
            $escaped_value = sprintf("'%s'", mysqli_real_escape_string(DPDatabase::get_connection(), $value));
        }
        else
        {
            $escaped_value = $value;
        }

        $sql = "
            SELECT *
            FROM non_activated_users
            WHERE $field=$escaped_value
        ";

        $result = mysqli_query(DPDatabase::get_connection(), $sql);
        if(!$result)
        {
            throw new UnexpectedValueException(mysqli_error(DPDatabase::get_connection()));
        }
        elseif(mysqli_num_rows($result) == 0)
        {
            throw new NonexistentNonactivatedUserException(sprintf(
                _('No non_activated_user found with %1$s = %2$s'),
                    $field, $value)
            );
        }
        elseif(mysqli_num_rows($result) > 1)
        {
            throw new NonuniqueNonactivatedUserException(sprintf(
                _('Multiple non_activated_users found with %1$s = %2$s'),
                    $field, $value)
            );
        }
        $this->table_row = mysqli_fetch_assoc($result);

        # MySQL does case-insensitive matches for strings, as well as
        # ignores trailing whitespace. If $strict is TRUE, and the field
        # is the username, require that the requested $username matches the
        # existing username in case and whitespace by doing a PHP string
        # compare.
        if($strict and $field == 'username' and $this->username != $value)
        {
            throw new NonexistentNonactivatedUserException(sprintf(
                _('No non_activated_user found with %1$s = %2$s'),
                    $field, $value)
            );
        }

        mysqli_free_result($result);
    }

    public function save()
    {
        $todays_date = time();

        // If this object doesn't have a unique ID, generate one before saving it to
        // the database.
        if(!isset($this->table_row["id"]))
        {
            // 16 random bytes turn into a 32-character hex string prefixed with 'userID'
            $this->table_row["id"] = "userID" . bin2hex(openssl_random_pseudo_bytes(16));
        }

        $referrer_details = $this->referrer == 'other' ? $this->referrer_details : '';

        // This function persists the object in the database, either by
        // creating a new row or updating an existing one. It does updates
        // with the use of an INSERT INTO / ON DUPLICATE KEY based on the
        // primary key (username). The use of ON DUPLICATE KEY creates the
        // possibility of an unlikely race condition where two simultaneous
        // registrations with the same ID would "succeed" where the second
        // request just saves over the prior one.
        $query = sprintf("
            INSERT INTO non_activated_users
            SET
                id = '%s',
                date_created = $todays_date,
                username = '%s',
                real_name = '%s',
                email = '%s',
                email_updates = %d,
                referrer = '%s',
                referrer_details = '%s',
                http_referrer = '%s',
                u_intlang = '%s',
                user_password = '%s'
            ON DUPLICATE KEY UPDATE
                real_name = '%s',
                email = '%s',
                email_updates = %d,
                referrer = '%s',
                referrer_details = '%s',
                http_referrer = '%s',
                u_intlang = '%s',
                user_password = '%s'
        ",  $this->table_row["id"],
            mysqli_real_escape_string(DPDatabase::get_connection(), $this->username),
            mysqli_real_escape_string(DPDatabase::get_connection(), $this->real_name),
            mysqli_real_escape_string(DPDatabase::get_connection(), $this->email),
            (int)$this->email_updates,
            mysqli_real_escape_string(DPDatabase::get_connection(), $this->referrer),
            mysqli_real_escape_string(DPDatabase::get_connection(), $referrer_details),
            mysqli_real_escape_string(DPDatabase::get_connection(), $this->http_referrer),
            mysqli_real_escape_string(DPDatabase::get_connection(), $this->u_intlang),
            mysqli_real_escape_string(DPDatabase::get_connection(), $this->user_password),
            // and for the updates
            mysqli_real_escape_string(DPDatabase::get_connection(), $this->real_name),
            mysqli_real_escape_string(DPDatabase::get_connection(), $this->email),
            (int)$this->email_updates,
            mysqli_real_escape_string(DPDatabase::get_connection(), $this->referrer),
            mysqli_real_escape_string(DPDatabase::get_connection(), $referrer_details),
            mysqli_real_escape_string(DPDatabase::get_connection(), $this->http_referrer),
            mysqli_real_escape_string(DPDatabase::get_connection(), $this->u_intlang),
            mysqli_real_escape_string(DPDatabase::get_connection(), $this->user_password)
        );

        $result = mysqli_query(DPDatabase::get_connection(), $query);

        if(!$result)
        {
            throw new UnexpectedValueException(mysqli_error(DPDatabase::get_connection()));
        }
    }

    public function delete()
    {
        if(!isset($this->id))
            return;

        mysqli_query(DPDatabase::get_connection(), "
            DELETE FROM non_activated_users
            WHERE id = '$this->id'
        ");
    }

    // static functions

    // Load a NonactivatedUser record by ID
    // e.g. $user = NonactivatedUser::load_from_id($id);
    public static function load_from_id($id)
    {
        $user = new NonactivatedUser();
        $user->load('id', $id);
        return $user;
    }
}

// vim: sw=4 ts=4 expandtab
