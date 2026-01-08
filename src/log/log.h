#ifndef LOG_H
#define LOG_H

/**
 * Types de messages pouvant être écrits dans le journal.
 */
enum LogType {
    LOG_INFO,    /**< Information générale. */
    LOG_WARNING, /**< Avertissement (comportement anormal mais non bloquant). */
    LOG_ERROR    /**< Erreur bloquante ou critique. */
};

/**
 * Initialise le système de journalisation.
 *
 * Ouvre le fichier de log et prépare les ressources nécessaires
 * à l’écriture des messages (formats, horodatage, etc.).
 *
 * \return 0 en cas de succès, 1 en cas d’erreur d’initialisation.
 */
int init_log(void);

/**
 * Écrit un message formaté dans le journal avec un certain niveau de log.
 *
 * Utilise un type parmi LogType pour classifier le message (info, warning, error)
 * et un format printf avec arguments variadiques pour le contenu.
 *
 * \param type    Type de message (LOG_INFO, LOG_WARNING, LOG_ERROR).
 * \param message Format de la chaîne (comme pour printf).
 * \param ...     Arguments supplémentaires correspondant au format.
 * \return 0 en cas de succès, une valeur non nulle en cas d’erreur d’écriture.
 */
int write_log(int type, const char *message, ...);


#endif // LOG_H