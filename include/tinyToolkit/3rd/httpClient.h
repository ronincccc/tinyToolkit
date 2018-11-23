#ifndef __TINY_TOOLKIT__3RD__HTTP_CLIENT__H__
#define __TINY_TOOLKIT__3RD__HTTP_CLIENT__H__


#ifdef TINY_TOOLKIT_CURL_SUPPORT


/**
 *
 *  作者: hm
 *
 *  说明: http客户端
 *
 */


#include <curl/curl.h>

#include "../common/common.h"


namespace tinyToolkit
{
	class TINY_TOOLKIT_API HTTPProxy
	{
	public:
		/**
		 *
		 * 析构函数
		 *
		 */
		~HTTPProxy()
		{
			Clear();
		}

		/**
		 *
		 * 清理
		 *
		 */
		void Clear()
		{
			enable = false;

			host.clear();
			user.clear();
			pass.clear();
			login.clear();

			type = CURLPROXY_HTTP;
		}

	public:
		bool enable{ false };

		std::string host{ };
		std::string user{ };
		std::string pass{ };
		std::string login{ };

		curl_proxytype type{ CURLPROXY_HTTP };
	};

	class TINY_TOOLKIT_API HTTPSession
	{
	public:
		/**
		 *
		 * 析构函数
		 *
		 */
		~HTTPSession()
		{
			Clear();
		}

		/**
		 *
		 * 清理
		 *
		 */
		void Clear()
		{
			count = 0;
			verbose = 0;
			errorCount = 0;
			failedCount = 0;
			successCount = 0;

			timeout = 0;
			receiveTimeout = 0;

			url.clear();
			user.clear();
			pass.clear();
			login.clear();
			caFile.clear();
			params.clear();
			cookie.clear();
			userAgent.clear();

			if (headers)
			{
				curl_slist_free_all(headers);

				headers = nullptr;
			}
		}

	public:
		uint32_t count{ 0 };
		uint32_t verbose{ 0 };
		uint32_t errorCount{ 0 };
		uint32_t failedCount{ 0 };
		uint32_t successCount{ 0 };

		std::time_t timeout{ 0 };
		std::time_t receiveTimeout { 0 };

		std::string url{ };
		std::string user{ };
		std::string pass{ };
		std::string login{ };
		std::string caFile{ };
		std::string params{ };
		std::string cookie{ };
		std::string userAgent{ };

		struct curl_slist * headers{ nullptr };
	};

	class TINY_TOOLKIT_API HTTPResponse
	{
	public:
		/**
		 *
		 * 析构函数
		 *
		 */
		~HTTPResponse()
		{
			Clear();
		}

		/**
		 *
		 * 清理
		 *
		 */
		void Clear()
		{
			code = 0;

			header.clear();
			content.clear();

			totalTime = 0.0;
			contentLength = 0.0;
		}

	public:
		uint32_t code{ 0 };

		std::string header{ };
		std::string content{ };

		std::double_t totalTime{ 0.0 };
		std::double_t contentLength{ 0.0 };
	};

	class TINY_TOOLKIT_API HTTPClient
	{
		enum class HTTP_OPTION_TYPE : uint8_t
		{
			GET,
			PUT,
			POST,
		};

	public:
		/**
		 *
		 * 构造函数
		 *
		 */
		HTTPClient() = default;

		/**
		 *
		 * 构造函数
		 *
		 * @param url 链接
		 *
		 */
		explicit HTTPClient(std::string url)
		{
			SetUrl(std::move(url));
		}

		/**
		 *
		 * 析构函数
		 *
		 */
		~HTTPClient()
		{
			Clear();
		}

		/**
		 *
		 * get操作(http)
		 *
		 * @return 操作结果
		 *
		 */
		bool Get()
		{
			return Launch(HTTP_OPTION_TYPE::GET);
		}

		/**
		 *
		 * put操作(http)
		 *
		 * @return 操作结果
		 *
		 */
		bool Put()
		{
			return Launch(HTTP_OPTION_TYPE::PUT);
		}

		/**
		 *
		 * post操作(http)
		 *
		 * @return 操作结果
		 *
		 */
		bool Post()
		{
			return Launch(HTTP_OPTION_TYPE::POST);
		}

		/**
		 *
		 * 最后一次错误信息
		 *
		 * @return 最后一次错误信息
		 *
		 */
		const char * LastError() const
		{
			return curl_easy_strerror(_errorCode);
		}

		/**
		 *
		 * 代理信息
		 *
		 * @return 代理信息
		 *
		 */
		const HTTPProxy & Proxy() const
		{
			return _proxy;
		}

		/**
		 *
		 * 会话信息
		 *
		 * @return 会话信息
		 *
		 */
		const HTTPSession & Session() const
		{
			return _session;
		}

		/**
		 *
		 * 响应信息
		 *
		 * @return 响应信息
		 *
		 */
		const HTTPResponse & Response() const
		{
			return _response;
		}

		/**
		 *
		 * 设置链接
		 *
		 * @param url 链接
		 *
		 */
		void SetUrl(std::string url)
		{
			_session.url = std::move(url);
		}

		/**
		 *
		 * 设置登陆信息
		 *
		 * @param user 帐号
		 * @param pass 密码
		 *
		 */
		void SetLogin(std::string user, std::string pass)
		{
			_session.user = std::move(user);
			_session.pass = std::move(pass);

			if (!_session.user.empty() || !_session.pass.empty())
			{
				_session.login = _session.user + ":" + _session.pass;
			}
		}

		/**
		 *
		 * 设置代理信息
		 *
		 * @param host 主机
		 * @param user 帐号
		 * @param pass 密码
		 * @param type 类型
		 *
		 */
		void SetProxy(std::string host, std::string user, std::string pass, curl_proxytype type = CURLPROXY_HTTP)
		{
			_proxy.type = type;
			_proxy.host = std::move(host);
			_proxy.user = std::move(user);
			_proxy.pass = std::move(pass);
			_proxy.enable = true;

			if (!_proxy.user.empty() || !_proxy.pass.empty())
			{
				_proxy.login = _proxy.user + ":" + _proxy.pass;
			}
		}

		/**
		 *
		 * 设置参数
		 *
		 * @param param 参数
		 *
		 */
		void SetParam(std::string param)
		{
			_session.params = std::move(param);
		}

		/**
		 *
		 * 设置ca证书路径
		 *
		 * @param caFile ca证书路径
		 *
		 */
		void SetCAFile(std::string caFile)
		{
			_session.caFile = std::move(caFile);
		}

		/**
		 *
		 * 设置cookie
		 *
		 * @param cookie cookie
		 *
		 */
		void SetCookie(std::string cookie)
		{
			_session.cookie = std::move(cookie);
		}

		/**
		 *
		 * 设置头部信息
		 *
		 * @param header 头部信息
		 *
		 */
		void SetHeader(const char * header)
		{
			if (_session.headers)
			{
				curl_slist_free_all(_session.headers);

				_session.headers = nullptr;
			}

			AddHeader(header);
		}

		/**
		 *
		 * 设置用户代理
		 *
		 * @param userAgent 用户代理
		 *
		 */
		void SetUserAgent(std::string userAgent)
		{
			_session.userAgent = std::move(userAgent);
		}

		/**
		 *
		 * 设置连接超时时间
		 *
		 * @param timeout 连接超时时间
		 *
		 */
		void SetConnectTimeout(std::time_t timeout)
		{
			_session.timeout = timeout;
		}

		/**
		 *
		 * 设置接收数据超时时间
		 *
		 * @param timeout 接收数据超时时间
		 *
		 */
		void SetReceiveTimeout(std::time_t timeout)
		{
			_session.receiveTimeout = timeout;
		}

		/**
		 * 添加参数
		 *
		 * @param param 参数
		 *
		 */
		void AddParam(const std::string & param)
		{
			_session.params += param;
		}

		/**
		 *
		 * 添加头部信息
		 *
		 * @param header 头部信息
		 *
		 */
		void AddHeader(const char * header)
		{
			_session.headers = curl_slist_append(_session.headers, header);
		}

		/**
		 *
		 * 启用详细信息
		 *
		 */
		void EnableVerbose()
		{
			_session.verbose = 1L;
		}

		/**
		 *
		 * 禁用详细信息
		 *
		 */
		void DisableVerbose()
		{
			_session.verbose = 0L;
		}

		/**
		 *
		 * 清理
		 *
		 */
		void Clear()
		{
			ClearProxy();
			ClearSession();
			ClearResponse();

			_errorCode = CURLE_OK;

			_isFirstWrite = true;

			if (_curl)
			{
				curl_easy_cleanup(_curl);

				_curl = nullptr;
			}
		}

		/**
		 *
		 * 清理代理信息
		 *
		 */
		void ClearProxy()
		{
			_proxy.Clear();
		}

		/**
		 *
		 * 清理会话信息
		 *
		 */
		void ClearSession()
		{
			_session.Clear();
		}

		/**
		 *
		 * 清理响应信息
		 *
		 */
		void ClearResponse()
		{
			_response.Clear();
		}

		/**
		 *
		 * 注销全局参数
		 *
		 */
		static void GlobalRelease()
		{
			curl_global_cleanup();
		}

		/**
		 *
		 * 初始化全局参数
		 *
		 * @return 是否初始化成功
		 *
		 */
		static bool GlobalInitialize()
		{
			return curl_global_init(CURL_GLOBAL_ALL) == CURLE_OK;
		}
	protected:
		/**
		 *
		 * 初始化
		 *
		 * @return 初始化状态
		 *
		 */
		bool Initialize()
		{
			_response.Clear();

			_curl = curl_easy_init();

			_errorCode = CURLE_OK;

			_isFirstWrite = true;

			SetShareDNSCache(_curl);

			if (_curl)
			{
				return true;
			}
			else
			{
				_errorCode = CURLE_FAILED_INIT;

				++_session.errorCount;

				return false;
			}
		}

		/**
		 *
		 * 解析响应代码
		 *
		 */
		void ParseResponseCode()
		{
			if (curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &_response.code) != CURLE_OK)
			{
				_response.code = 0;
			}
		}

		/**
		 *
		 * 解析响应数据长度
		 *
		 */
		void ParseResponseLength()
		{
			if (curl_easy_getinfo(_curl, CURLINFO_CONTENT_LENGTH_DOWNLOAD, &_response.contentLength) == CURLE_OK)
			{
				if (_response.contentLength > 0)
				{
					_response.content.reserve(static_cast<std::size_t>(_response.contentLength));
				}
			}
			else
			{
				_response.contentLength = 0.0;
			}
		}

		/**
		 *
		 * 设置代理信息
		 *
		 */
		void SetProxy()
		{
			if (_proxy.enable)
			{
				curl_easy_setopt(_curl, CURLOPT_PROXY, _proxy.host.c_str());  /// 代理主机
				curl_easy_setopt(_curl, CURLOPT_PROXYTYPE, _proxy.type);  /// 代理类型
				curl_easy_setopt(_curl, CURLOPT_PROXYAUTH, CURLAUTH_ANY);  /// 代理认证模式

				if (!_proxy.login.empty())
				{
					curl_easy_setopt(_curl, CURLOPT_PROXYUSERPWD, _proxy.login.c_str());  /// 代理登陆信息
				}
			}
		}

		/**
		 *
		 * 设置会话信息
		 *
		 * @param option 操作类型
		 *
		 */
		void SetSession(HTTP_OPTION_TYPE option)
		{
			if (_session.verbose)
			{
				curl_easy_setopt(_curl, CURLOPT_VERBOSE, _session.verbose);  /// 启用时汇报所有的信息
			}

			if (_session.headers)
			{
				curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _session.headers);  ///  设置头信息
			}

			if (_session.timeout == 0)
			{
				curl_easy_setopt(_curl, CURLOPT_CONNECTTIMEOUT, 3);  /// 连接超时时间
			}
			else
			{
				curl_easy_setopt(_curl, CURLOPT_CONNECTTIMEOUT, _session.timeout);  /// 连接超时时间
			}

			if (_session.receiveTimeout == 0)
			{
				curl_easy_setopt(_curl, CURLOPT_TIMEOUT, 10);  /// 接收数据时超时时间
			}
			else
			{
				curl_easy_setopt(_curl, CURLOPT_TIMEOUT, _session.receiveTimeout);  /// 接收数据时超时时间
			}

			if (!_session.url.empty())
			{
				curl_easy_setopt(_curl, CURLOPT_URL, _session.url.c_str());  /// 设置请求的地址
			}

			if (!_session.login.empty())
			{
				curl_easy_setopt(_curl, CURLOPT_USERPWD, _session.login.c_str());  /// 设置登陆信息
			}

			if (_session.caFile.empty())
			{
				curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 0L);  /// 不验证证书
				curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYHOST, 0L);  /// 不验证证书中是否设置域名
			}
			else
			{
				curl_easy_setopt(_curl, CURLOPT_CAINFO, _session.caFile.c_str());  /// CA根证书

				curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYPEER, 1L);  /// 只信任CA颁布的证书
				curl_easy_setopt(_curl, CURLOPT_SSL_VERIFYHOST, 2L);  /// 检查证书中是否设置域名, 并且是否与提供的主机名匹配
			}

			if (!_session.cookie.empty())
			{
				curl_easy_setopt(_curl, CURLOPT_COOKIE, _session.cookie.c_str());  /// 设置cookie
			}

			if (!_session.userAgent.empty())
			{
				curl_easy_setopt(_curl, CURLOPT_USERAGENT, _session.userAgent.c_str());  /// 设置用户代理
			}

			switch (option)
			{
				case HTTP_OPTION_TYPE::GET:
				{
					curl_easy_setopt(_curl, CURLOPT_HTTPGET, 1L);  /// 设置get操作

					break;
				}

				case HTTP_OPTION_TYPE::PUT:
				{
					curl_easy_setopt(_curl, CURLOPT_PUT, 1L);  /// 设置put操作

					if (!_session.params.empty())
					{
						curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, _session.params.c_str());  /// 设置提交的数据
					}

					break;
				}

				case HTTP_OPTION_TYPE::POST:
				{
					curl_easy_setopt(_curl, CURLOPT_POST, 1L);  /// 设置post操作

					if (!_session.params.empty())
					{
						curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, _session.params.c_str());  /// 设置提交的数据
					}

					break;
				}

				default:
				{
					break;
				}
			}

			/// 设置获取返回内容的缓冲区
			curl_easy_setopt(_curl, CURLOPT_WRITEDATA, this);
			/// 设置获取返回内容的回调函数
			curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, WriteFunction);

			/// 设置获取头部信息的缓冲区
			curl_easy_setopt(_curl, CURLOPT_HEADERDATA, this);
			/// 设置获取头部信息的回调函数
			curl_easy_setopt(_curl, CURLOPT_HEADERFUNCTION, HeaderFunction);

			/// http验证
			curl_easy_setopt(_curl, CURLOPT_HTTPAUTH, CURLAUTH_ANY);
			/// 当多个线程都使用超时处理的时候, 同时主线程中有sleep或是wait等操作. 如果不设置这个选项, 将会发信号打断wait从而导致程序退出.
			curl_easy_setopt(_curl, CURLOPT_NOSIGNAL, 1L);
			/// 查找次数，防止查找太深
			curl_easy_setopt(_curl, CURLOPT_MAXREDIRS, 3L);
			/// 默认情况下完成一个任务以后, 出于重用连接的考虑不会马上关闭, 如果没有新的TCP请求来重用这个连接, 那么只能等到CLOSE_WAIT超时
			/// 这个时间默认在7200秒甚至更高, 太多的CLOSE_WAIT连接会导致性能问题
			curl_easy_setopt(_curl, CURLOPT_FORBID_REUSE, 1L);
			/// 返回的头部中有Location(一般直接请求的url没找到, 301/302跳转), 则继续请求Location对应的数据
			curl_easy_setopt(_curl, CURLOPT_FOLLOWLOCATION, 1L);
		}

		/**
		 *
		 * http操作
		 *
		 * @param option 操作类型
		 *
		 * @return 操作结果
		 *
		 */
		bool Launch(HTTP_OPTION_TYPE option)
		{
			++_session.count;

			if (!Initialize())
			{
				return false;
			}

			SetProxy();
			SetSession(option);

			_errorCode = curl_easy_perform(_curl);

			if (_errorCode == CURLE_OK)
			{
				++_session.successCount;
			}
			else
			{
				++_session.failedCount;
			}

			if (_curl)
			{
				curl_easy_cleanup(_curl);

				_curl = nullptr;
			}

			return _errorCode == CURLE_OK;
		}

		/**
		 *
		 * 设置共享DNS解析
		 *
		 * @param curl 待设置句柄
		 *
		 */
		void SetShareDNSCache(CURL * curl)
		{
			static CURLSH * shareHandle = nullptr;

			if (shareHandle == nullptr)
			{
				shareHandle = curl_share_init();

				/**
				 *
				 * curl_easy_perform每次执行时, 都会启动一个线程用于做DNS解析
				 *
				 * 每一个handle有一个DNS cache, 默认超时时间为60秒, 如果不重用handle的话, 这个dns cache在连接完成后就被析构了
				 *
				 * 每一个handle执行前, 使用CURLOPT_SHARE选项把这个shareHandle设置给handle, 这样多个handle就可以共用同一个DNS cache了
				 *
				 * 在超时时间内, 每次curl_easy_perform执行时不会再启动解析DNS的线程了, 效率大大提高
				 *
				 */
				curl_share_setopt(shareHandle, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);  /// 设置共享DNS cache
			}

			curl_easy_setopt(curl, CURLOPT_SHARE, shareHandle);  /// 设置共享句柄
			curl_easy_setopt(curl, CURLOPT_DNS_CACHE_TIMEOUT, 60 * 5);  /// 设置超时时间
		}

		/**
		 *
		 * 响应回调函数
		 *
		 * @param buffer 响应数据缓冲区
		 * @param size 响应数据块大小
		 * @param nmemb 响应数据块个数
		 * @param stream 存储响应数据地址
		 *
		 * @return 响应数据大小
		 *
		 */
		static std::size_t WriteFunction(void * buffer, std::size_t size, std::size_t nmemb, void * stream)
		{
			auto count = nmemb * size;

			auto * client = reinterpret_cast<HTTPClient *>(stream);

			if (client == nullptr)
			{
				return count;
			}

			if (client->_isFirstWrite)
			{
				client->_isFirstWrite = false;

				client->ParseResponseCode();
				client->ParseResponseLength();
			}

			client->_response.content.append(reinterpret_cast<const char *>(buffer), count);

			return count;
		}

		/**
		 *
		 * 响应头部信息回调函数
		 *
		 * @param buffer 响应数据缓冲区
		 * @param size 响应数据块大小
		 * @param nmemb 响应数据块个数
		 * @param stream 存储响应数据地址
		 *
		 * @return 响应数据大小
		 *
		 */
		static std::size_t HeaderFunction(void * buffer, std::size_t size, std::size_t nmemb, void * stream)
		{
			auto count = nmemb * size;

			auto * client = reinterpret_cast<HTTPClient *>(stream);

			if (client == nullptr)
			{
				return count;
			}

			client->_response.header.append(reinterpret_cast<const char *>(buffer), count);

			return count;
		}

	protected:
		bool _isFirstWrite{ true };

		CURL * _curl{ nullptr };

		CURLcode _errorCode{ CURLE_OK };

		HTTPProxy _proxy;
		HTTPSession _session;
		HTTPResponse _response;
	};
}


#endif // TINY_TOOLKIT_CURL_SUPPORT


#endif // __TINY_TOOLKIT__3RD__HTTP_CLIENT__H__
