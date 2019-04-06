#pragma once

enum class Error
{
	NoError,
	CorruptedArchive
};

class Serializer
{
	static constexpr char Separator = ' ';
	std::ostream &out_;
public:
	explicit Serializer(std::ostream& out)
		: out_(out)
	{
	}

	template <class T>
	Error save(T& object)
	{
		return object.serialize(*this);
	}

	template <class... Args>
	Error operator()(Args... args)
	{
		return process(args...);
	}

private:
	Error process(bool& val)
	{
		if(val)
			out_ << "true" << Separator;
		else
			out_ << "false" << Separator;
		return Error::NoError;
	}
	Error process(uint64_t& val)
	{
		out_ << val << Separator;
		return Error::NoError;
	}

	template <class T, class... Args>
	Error process(T&& val, Args&&... args)
	{
		process(val);
		process(std::forward<Args>(args)...);
		return Error::NoError;
	}
};

class Deserializer
{
	std::istream& in_;
public:
	explicit Deserializer(std::istream& in)
		: in_(in)
	{
	}

	template <class T>
	Error load(T& object)
	{
		return object.serialize(*this);
	}

	template <class... Args>
	Error operator()(Args&... args)
	{
		return process(args...);
	}

private:
	Error process(bool& val)
	{
		std::string text;
		in_ >> text;

		if (text == "true")
			val = true;
		else if (text == "false")
			val = false;
		else
			return Error::CorruptedArchive;

		return Error::NoError;
	}
	Error process(uint64_t& val)
	{
		std::string text;
		in_ >> text;

		if (text[0]=='-')
			return Error::CorruptedArchive;
		try {
			val = stoul(text);
		}
		catch (const invalid_argument& error)
		{
			return Error::CorruptedArchive;
		}

		return Error::NoError;
	}


	template <class T, class... Args>
	Error process(T&& val, Args&& ... args)
	{
		auto er=process(val);
		if(er==Error::NoError)
			return process(std::forward<Args>(args)...);
		return Error::CorruptedArchive;
	}
};

