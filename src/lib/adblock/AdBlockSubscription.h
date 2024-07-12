#ifndef ADBLOCKSUBSCRIPTION_H
#define ADBLOCKSUBSCRIPTION_H

#include "AdBlockFilter.h"

#include <memory>
#include <vector>
#include <QDateTime>
#include <QString>
#include <QUrl>

namespace adblock
{

class AdBlockManager;

/**
 * @class Subscription
 * @ingroup AdBlock
 * @brief Manages a list of filter rules that are defined
 *        according to the AdBlockPlus and/or uBlock Origin
 *        filter specifications.
 */
class Subscription
{
    friend class FilterContainer;
    friend class AdBlockManager;
    friend class AdBlockRequestHandler;

public:
    /// Constructs the Subscription object
    Subscription();

    /// Constructs the Subscription with the path to its data file
    Subscription(const QString &dataFile);

    /// Copy constructor (forbid)
    Subscription(const Subscription &other) = delete;

    /// Move constructor
    Subscription(Subscription &&other) noexcept;

    /// Copy assignment operator
    Subscription &operator =(const Subscription &other) = delete;

    /// Move assignment operator
    Subscription &operator =(Subscription &&other) noexcept;

    /// Destructor
    ~Subscription();

    /// Returns true if the subscription is enabled, false if disabled
    bool isEnabled() const;

    /**
     * @brief Toggles the state of the subscription
     * @param value If true, enables the subscription. If false, disables the subscription
     */
    void setEnabled(bool value);

    /// Returns the name of the subscription, or the file name if a name has not been given
    const QString &getName() const;

    /// Returns the source URL of the subscription file
    const QUrl &getSourceUrl() const;

    /// Returns the time of the subscription's last update
    const QDateTime &getLastUpdate() const;

    /// Returns the time of the next update
    const QDateTime &getNextUpdate() const;

protected:
    /// Loads the filters from the subscription file
    void load(AdBlockManager *adBlockManager);

    /// Sets the time of the last update of the subscription file
    void setLastUpdate(const QDateTime &date);

    /// Sets the time when the subscription will be updated
    void setNextUpdate(const QDateTime &date);

    /// Sets the source URL of the subscription file. Used for updates
    void setSourceUrl(const QUrl &source);

    // Sets the name
    void setName(const QString &name) {
      m_name = name;
    }

    /// Returns the number of filters that belong to the subscription
    size_t getNumFilters() const;

    /// Returns the filter at the given index
    Filter *getFilter(size_t index);

    /// Returns the absolute path of the subscription file
    const QString &getFilePath() const;

    /// Updates the path of the subscription file - called after completion of an update if the file name is different
    void setFilePath(const QString &filePath);

private:
    /// True if subscription is enabled, false if else
    bool m_enabled;

    /// Path to the subscription file
    QString m_filePath;

    /// Name of the subscription
    QString m_name;

    /// Source URL of the subscription file
    QUrl m_sourceUrl;

    /// Time of the last update to the subscription file
    QDateTime m_lastUpdate;

    /// Time when the subscription should be updated
    QDateTime m_nextUpdate;

    /// Container of AdBlock Filters that belong to the subscription
    std::vector< std::unique_ptr<Filter> > m_filters;
};

}

#endif // ADBLOCKSUBSCRIPTION_H
